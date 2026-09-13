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
#pragma once

#ifdef ARDUINO
#include "Arduino.h"
#endif

// #define RAK3112_LORA_AUDIO
#define ESP32S3_DEVKITC_1


// #if defined ARDUINO_ESP32_DEV
// #define LED_BUILTIN 2 // for ESP32, the on-board LED is connected to GPIO2
// #pragma message "LED_BUILTIN is not defined, define it as 2 for ESP32"
// // #elif defined ARDUINO_XIAO_ESP32C3
// // #define LED_BUILTIN 10 // for XIAO ESP32C3, the on-board LED is connected to GPIO10
// // #pragma message "LED_BUILTIN is not defined, define it as 10 for XIAO ESP32C3"
// #endif

///////////////////////////////////////////////////////////////////////////////
// define application specific pins here, such as LED pin, button pin, etc.
#if defined(ESP32S3_DEVKITC_1)
#undef LED_BUILTIN                  // no on-board led on ESP32S3-DevKitC-1
#define PIN_BOOT GPIO_NUM_0         // swBoot

#define PIN_WS2812 GPIO_NUM_48      // on-board WS2812 on ESP32S3-KevKitC-1


#elif defined(RAK3112_LORA_AUDIO)
#define PIN_BOOT GPIO_NUM_0         // swBoot

#endif

