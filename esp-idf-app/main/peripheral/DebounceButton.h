/* Copyright 2025 teamprof.net@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#pragma once

#ifdef ARDUINO
#include "Arduino.h"
#include <FunctionalInterrupt.h>
#elif defined ESP_PLATFORM
#include "esp_intr_alloc.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#endif

#include "../ArduProfApp.h"
#include "../AppEvent.h"
#include "./DebounceDef.h"
#include "./DebounceTimer.h"

#if defined ARDUPROF_FREERTOS 
class DebounceTimer;

class DebounceButton : public ardufreertos::MessageQueue
{
public:
#ifdef ARDUINO
    DebounceButton(uint8_t pin,
                   uint8_t activeState,
                   uint8_t ioMode,
                   QueueHandle_t queue) : 
#elif defined ESP_PLATFORM
    DebounceButton(gpio_num_t pin,
                   uint8_t activeState,
                   gpio_mode_t ioMode,
                   QueueHandle_t queue) : 
#endif
                        MessageQueue(queue),
                        debounceCount(0),
                        debounceActive(false),
                        pinStateActive(activeState),
                        isIntrEnable(false),
                        _eventValue(EventNull),
                        _buttonClick(EventNull),
                        _buttonDoubleClick(EventNull),
                        _buttonLongPress(EventNull),
                        _debounceTimer(nullptr),
                        _clickCount(0),
                        _last_state(!activeState),
                        _last_isr_ms(0),
                        _PIN(pin)
    {
        // _timeBegin, _timeEnd;

#ifdef ARDUINO
        pinMode(_PIN, ioMode);
#elif defined ESP_PLATFORM
        gpio_reset_pin(_PIN);
        gpio_set_direction(_PIN, ioMode);
#endif
    }

    ~DebounceButton()
    {
        disableInterrupt();
    }

    bool init(int16_t evValue, int16_t clickValue, int16_t doubleClickValue, int16_t longPressValue)
    {
        _eventValue = evValue;
        _buttonClick = clickValue;
        _buttonDoubleClick = doubleClickValue;
        _buttonLongPress = longPressValue;
        return true;
    }

#if defined ARDUINO_ARCH_ESP32
    void enableInterrupt(uint8_t intrMode)
    {
        if (!isIntrEnable)
        {
            attachInterrupt(digitalPinToInterrupt(_PIN), std::bind(&DebounceButton::isr, this), intrMode);
            isIntrEnable = true;
        }
    }
#elif defined ESP_PLATFORM
    void enableInterrupt(gpio_int_type_t intrMode)
    {
        if (!isIntrEnable)
        {
            // gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
            gpio_install_isr_service(0);
            gpio_isr_handler_add(_PIN, DebounceButton::isr, (void*)this);
            gpio_set_intr_type(_PIN, intrMode);
            isIntrEnable = true;
        }
    }
#elif defined ARDUINO_RASPBERRY_PI_PICO_2 || defined ARDUINO_RASPBERRY_PI_PICO
    void enableInterrupt(PinStatus intrMode)
    {
        if (!isIntrEnable)
        {
            attachInterrupt(digitalPinToInterrupt(_PIN), isr, intrMode, this);
            isIntrEnable = true;
        }
    }
#endif

    void disableInterrupt(void)
    {
#if defined ARDUINO_ARCH_ESP32 || defined ARDUINO_RASPBERRY_PI_PICO_2 || defined ARDUINO_RASPBERRY_PI_PICO
        if (isIntrEnable)
        {
            detachInterrupt(digitalPinToInterrupt(_PIN));
            isIntrEnable = false;
        }
#elif defined ESP_PLATFORM
        if (isIntrEnable)
        {
            gpio_set_intr_type(_PIN, GPIO_INTR_DISABLE);
            isIntrEnable = false;
        }
#endif
    }

    void setDebounceActive(bool active)
    {
        debounceCount = 0;
        debounceActive = active;
    }

    bool isDebounceActive(void)
    {
        return debounceActive;
    }

    void onEventIsr(uint8_t value, uint32_t ms)
    {
        if (value == getActiveState())
        {
            _timeBegin = ms;
            if (_clickCount == 0)
            {
                setDebounceActive(true);
                if (_debounceTimer)
                {
                    _debounceTimer->start();
                }
            }
        }
        else if (isDebounceActive())
        {
            _timeEnd = ms;
            uint32_t delta = (_timeEnd > _timeBegin) ? (_timeEnd - _timeBegin) : (_timeBegin - _timeEnd);
            if (delta > DebounceDuration)
            {
                _clickCount++;
            }
        }
    }

    void onEventTimer(void)
    {
        if (!isDebounceActive())
        {
            return;
        }

#ifdef ARDUINO
        if (digitalRead(_PIN) == pinStateActive)
#elif defined ESP_PLATFORM
        if (gpio_get_level(_PIN) == pinStateActive)
#endif
        {
            if (debounceCount >= LongPressDuration)
            {
                _clickCount = 0;
                setDebounceActive(false);
                sendMessageToTask(_eventValue, _buttonLongPress, _PIN);
            }
            else
            {
                debounceCount += DebounceTimerInterval;
            }
        }
        else if (debounceCount >= DoubleClickDuration)
        {
            if (_clickCount == 1)
            {
                sendMessageToTask(_eventValue, _buttonClick, _PIN);
            }
            else if (_clickCount == 2)
            {
                sendMessageToTask(_eventValue, _buttonDoubleClick, _PIN);
            }

            _clickCount = 0;
            setDebounceActive(false);
        }
        else
        {
            debounceCount += DebounceTimerInterval;
            // setDebounceActive(false);
        }
    }

#ifdef ARDUINO
    uint8_t getPin(void)
#elif defined ESP_PLATFORM
    gpio_num_t getPin(void)
#endif
    {
        return _PIN;
    }
    uint8_t getActiveState(void)
    {
        return pinStateActive;
    }
    int read(void)
    {
#ifdef ARDUINO
        return digitalRead(_PIN);
#elif defined ESP_PLATFORM
        return gpio_get_level(_PIN);
#endif
    }

    uint16_t getClickCount(void)
    {
        return _clickCount;
    }

protected:
    uint32_t debounceCount;
    bool debounceActive;
    uint8_t pinStateActive;
    bool isIntrEnable;

private:

#ifdef ARDUINO
    static void isr(DebounceButton *instance)
    {
        if (instance)
        {
            auto pin = instance->getPin();
            uint32_t ms = millis();
            uint8_t state = digitalRead(pin);
            // soft Schmitt-triggers
            if (instance->_last_state != state)
            {
                instance->_last_isr_ms = ms;
                instance->_last_state = state;
                instance->sendMessageFromIsrToTask(EventGpioISR, pin, state, ms);
            }
        }
    }
#elif defined ESP_PLATFORM
    static void /*IRAM_ATTR*/ isr(void *param)
    {
        auto instance = reinterpret_cast<DebounceButton *>(param);
        if (instance)
        {
            auto pin = instance->getPin();
            uint32_t ms = esp_timer_get_time() / 1000;;
            uint8_t state = gpio_get_level(pin);
            // soft Schmitt-triggers
            if (instance->_last_state != state)
            {
                instance->_last_isr_ms = ms;
                instance->_last_state = state;
                instance->sendMessageFromIsrToTask(EventGpioISR, pin, state, ms);
            }
        }
    }
#endif

    int16_t _eventValue;
    int16_t _buttonClick;
    int16_t _buttonDoubleClick;
    int16_t _buttonLongPress;

    friend DebounceTimer;
    DebounceTimer *_debounceTimer;

    uint16_t _clickCount;
    uint32_t _timeBegin, _timeEnd;
    uint8_t _last_state;
    uint32_t _last_isr_ms;

#ifdef ARDUINO
    const uint8_t _PIN;
#elif defined ESP_PLATFORM
    const gpio_num_t _PIN;
#endif
};
#endif