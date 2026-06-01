/* Copyright 2026 teamprof.net@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdbool.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/types.h>
#include <zephyr/device.h>
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/sys/mem_stats.h>

#if defined(CONFIG_SOC_NRF52840_QIAA) || defined(CONFIG_SOC_NRF52840)
    // nRF52840 Specific Code
    #include <hal/nrf_ficr.h>
#elif defined(CONFIG_SOC_RP2350)
    // RP2350 Specific Code
    #include <hardware/regs/addressmap.h>
#else
    #error "Unsupported MCU"
#endif

#include "../peripheral/led_usr.h"
#include "./QueueMain.h"
#include "../AppVersion.h"

// LOG_MODULE_REGISTER(QueueMain, LOG_LEVEL_INF);
LOG_MODULE_REGISTER(QueueMain, LOG_LEVEL_DBG);

///////////////////////////////////////////////////////////////////////

// K_TIMER_DEFINE(QueueMain::_timer1Hz, QueueMain::_timerExpiryHandler,
//                QueueMain::_timerStopHandler);

///////////////////////////////////////////////////////////////////////
#define TASK_QUEUE_SIZE 16 // message queue size for app task
#define QUEUENAME CONCAT(QueueMain, taskQueue)

///////////////////////////////////////////////////////////////////////
K_MSGQ_DEFINE(QUEUENAME, sizeof(Message), TASK_QUEUE_SIZE, alignof(uint32_t));

QueueMain *QueueMain::_instance = NULL;

///////////////////////////////////////////////////////////////////////
QueueMain::QueueMain() : zephyros::MessageBus(&QUEUENAME), _ledState(false)
{
    _handlerMap = {
        __EVENT_MAP(QueueMain, EventSystem),

        // {EventNull, &QueueMain::handlerEventNull},
        __EVENT_MAP(QueueMain, EventNull),
    };
}

QueueMain *QueueMain::getInstance(void)
{
    if (!_instance)
    {
        static QueueMain instance;
        _instance = &instance;
    }
    return _instance;
}

void QueueMain::start(void *ctx)
{
    MessageBus::start(ctx);

    printChipInfo();

    int ret = led_usr::init();
    if (ret != 0)
    {
        LOG_ERR("Failed to initialize led_usr: %d", ret);
    }

    k_timer_init(&_timer1Hz, QueueMain::_timerExpiryHandler, QueueMain::_timerStopHandler);
    k_timer_start(&_timer1Hz, K_MSEC(1000), K_SECONDS(1));
}

void QueueMain::onMessage(const Message &msg)
{
    auto func = _handlerMap[msg.event];
    if (func)
    {
        (this->*func)(msg);
    }
    else
    {
        LOG_DBG("Unsupported event=%hd, iParam=%hd, uParam=%hu, lParam=%u",
                msg.event, msg.iParam, msg.uParam, msg.lParam);
    }
}

///////////////////////////////////////////////////////////////////////
bool QueueMain::getLedState(void) { return _ledState; }
void QueueMain::setLedState(bool ledState)
{
    _ledState = ledState;
    LOG_DBG("_ledState=%d", (int)_ledState);

    auto ret = led_usr::set(_ledState);
    if (ret < 0)
    {
        LOG_ERR("gpio_pin_set_dt() returns %d", ret);
    }
}

void QueueMain::toggleLedState(void) { setLedState(!_ledState); }

///////////////////////////////////////////////////////////////////////

// void QueueMain::handlerEventSystem(const Message &msg)
__EVENT_FUNC_DEFINITION(QueueMain, EventSystem, msg)
{
    // LOG_DBG("EventSystem(%hd), iParam=%hd, uParam=%hu, lParam=0x%08x",
    // msg.event, msg.iParam, msg.uParam, msg.lParam);
    enum SystemTriggerSource src =
        static_cast<enum SystemTriggerSource>(msg.iParam);
    switch (src)
    {
    case SysSoftwareTimer:
        handlerSoftwareTimer((k_timer *)(msg.lParam));
        break;
    default:
        LOG_DBG("unsupported SystemTriggerSource=%hd", src);
        break;
    }
}
// void QueueMain::handlerEventNull(const Message &msg)
__EVENT_FUNC_DEFINITION(QueueMain, EventNull, msg)
{
    LOG_DBG("EventNull(%hd), iParam=%hd, uParam=%hu, lParam=%u", msg.event,
            msg.iParam, msg.uParam, msg.lParam);
}

void QueueMain::handlerSoftwareTimer(k_timer *timer)
{
    if (timer == &_timer1Hz)
    {
        // LOG_DBG("timer1Hz");
        toggleLedState();
    }
    else
    {
        LOG_DBG("unsupported timer=%p", timer);
    }
}

///////////////////////////////////////////////////////////////////////
#if defined(CONFIG_SOC_NRF52840_QIAA) || defined(CONFIG_SOC_NRF52840)
void QueueMain::printChipInfo(void)
{
    LOG_INF("===============================================================================");
    LOG_INF("App Firmware version=%s", AppVersion::getFirmwareVersionString());

    // Hardware Variant
    // If INFO.VARIANT fails, try just NRF_FICR->VARIANT
    uint32_t variant = NRF_FICR->INFO.VARIANT;
    LOG_INF("Variant:     %c%c%c%c", 
           (char)(variant >> 24), (char)(variant >> 16), 
           (char)(variant >> 8),  (char)variant);
    // Part Number
    LOG_INF("MCU:         nRF%x", NRF_FICR->INFO.PART);

    // Flash and RAM 
    // In many newer nRF52 headers, these are at the top level of NRF_FICR
    // or renamed within INFO. Let's use the absolute direct registers:
    uint32_t codepagesize = NRF_FICR->CODEPAGESIZE; 
    uint32_t codesize = NRF_FICR->CODESIZE;
    
    uint32_t flash_kb = (codesize * codepagesize) / 1024;
    uint32_t ram_kb = NRF_FICR->INFO.RAM; 
    
    LOG_INF("Flash Size:  %d KB", flash_kb);
    LOG_INF("RAM Size:    %d KB", ram_kb);

    // Device ID
    LOG_INF("Device ID:   %08X%08X", NRF_FICR->DEVICEID[1], NRF_FICR->DEVICEID[0]);

    LOG_INF("===============================================================================");
}
#endif

void QueueMain::_timerExpiryHandler(struct k_timer *timer_id)
{
    auto instance = QueueMain::getInstance();
    instance->postEvent(EventSystem, SysSoftwareTimer, 0, (uint32_t)timer_id);
}
void QueueMain::_timerStopHandler(struct k_timer *timer)
{
    LOG_DBG("timer=%p", timer);
}