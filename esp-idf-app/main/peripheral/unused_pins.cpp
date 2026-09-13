/* Copyright 2026 teamprof.net@gmail.com
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
#include "driver/gpio.h" 
 
#include "unused_pins.h"

static const gpio_num_t unusedPins[] = {
#if defined(CONFIG_IDF_TARGET_ESP32S3)
    // PIO_NUM_0,      // BOOT
    GPIO_NUM_1,
    GPIO_NUM_2,
    GPIO_NUM_3,
    GPIO_NUM_4,
    GPIO_NUM_5,
    GPIO_NUM_6,
    GPIO_NUM_7,
    GPIO_NUM_8,     // 
    GPIO_NUM_9,     // 
    GPIO_NUM_10,    
    GPIO_NUM_11,    
    GPIO_NUM_12,    
    GPIO_NUM_13,    
    GPIO_NUM_14,
    GPIO_NUM_15,
    GPIO_NUM_16,
    GPIO_NUM_17,
    GPIO_NUM_18,
    // GPIO_NUM_19,        // USB_N
    // GPIO_NUM_20,        // USB_P
    GPIO_NUM_21,
    
    GPIO_NUM_35,
    GPIO_NUM_36,
    GPIO_NUM_37,
    GPIO_NUM_38,
    GPIO_NUM_39,
    GPIO_NUM_40,
    GPIO_NUM_41,
    GPIO_NUM_42,
    // GPIO_NUM_43,        // TX0
    // GPIO_NUM_44,        // RX0
    GPIO_NUM_45,
    GPIO_NUM_46,
    GPIO_NUM_47,
    GPIO_NUM_48,
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
#elif defined(CONFIG_IDF_TARGET_ESP32)
#endif
};

namespace unused_pins {
    void setup(void)
    {
        for (gpio_num_t pin : unusedPins) {
            gpio_set_direction(pin, GPIO_MODE_DISABLE);
            gpio_pullup_dis(pin);
            gpio_pulldown_dis(pin);
        }
    }
};