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

#if defined ARDUINO

#elif defined ARDUPROF_PICO_C

#include <stdio.h>
#include "pico/stdlib.h"

// Define Log Levels exactly like ESP-IDF
#define APP_LOG_NONE    0
#define APP_LOG_ERROR   1
#define APP_LOG_WARN    2
#define APP_LOG_INFO    3
#define APP_LOG_DEBUG   4
#define APP_LOG_VERBOSE 5

// Set Global Log Level (Change this to lower/higher verbosity as needed)
#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL APP_LOG_DEBUG
#endif

// ANSI Terminal Colors matching ESP32
#define LOG_CLR_E "\033[0;31m"  // Red
#define LOG_CLR_W "\033[0;33m"  // Yellow
#define LOG_CLR_I "\033[0;32m"  // Green
#define LOG_CLR_D "\033[0;36m"  // Cyan
#define LOG_CLR_V "\033[0;36m"  // Cyan
#define LOG_CLR_RESET "\033[0m"

// Drop-in macro definitions
#if LOG_LOCAL_LEVEL >= APP_LOG_ERROR
#define APP_LOGE(tag, format, ...) printf(LOG_CLR_E "E [%s.L" STR(__LINE__) "] %s: " format LOG_CLR_RESET "\n", tag, __func__, ##__VA_ARGS__)
// #define APP_LOGE(tag, format, ...) printf(LOG_CLR_E "E %s: " format LOG_CLR_RESET "\n", tag, ##__VA_ARGS__)
#else
#define APP_LOGE(tag, format, ...)
#endif

#if LOG_LOCAL_LEVEL >= APP_LOG_WARN
#define APP_LOGW(tag, format, ...) printf(LOG_CLR_W "W [%s.L" STR(__LINE__) "] %s: " format LOG_CLR_RESET "\n", tag, __func__, ##__VA_ARGS__)
// #define APP_LOGW(tag, format, ...) printf(LOG_CLR_W "W %s: " format LOG_CLR_RESET "\n", tag, ##__VA_ARGS__)
#else
#define APP_LOGW(tag, format, ...)
#endif

#if LOG_LOCAL_LEVEL >= APP_LOG_INFO
#define APP_LOGI(tag, format, ...) printf(LOG_CLR_I "I [%s.L" STR(__LINE__) "] %s: " format LOG_CLR_RESET "\n", tag, __func__, ##__VA_ARGS__)
// #define APP_LOGI(tag, format, ...) printf(LOG_CLR_I "I %s: " format LOG_CLR_RESET "\n", tag, ##__VA_ARGS__)
#else
#define APP_LOGI(tag, format, ...)
#endif

#if LOG_LOCAL_LEVEL >= APP_LOG_DEBUG
#define APP_LOGD(tag, format, ...) printf(LOG_CLR_D "D [%s.L" STR(__LINE__) "] %s: " format LOG_CLR_RESET "\n", tag, __func__, ##__VA_ARGS__)
// #define APP_LOGD(tag, format, ...) printf(LOG_CLR_D "D %s: " format LOG_CLR_RESET "\n", tag, ##__VA_ARGS__)
#else
#define APP_LOGD(tag, format, ...)
#endif

#if LOG_LOCAL_LEVEL >= APP_LOG_VERBOSE
// #define APP_LOGV(tag, format, ...) printf(LOG_CLR_V "V [%s.L" STR(__LINE__) "] %s: " format LOG_CLR_RESET "\n", tag, __func__, ##__VA_ARGS__)
#define APP_LOGV(tag, format, ...) printf(LOG_CLR_V "V %s: " format LOG_CLR_RESET "\n", tag, ##__VA_ARGS__)
#else
#define APP_LOGV(tag, format, ...)
#endif

#elif defined ESP_PLATFORM
#include "esp_log.h"

#define APP_LOGE ESP_LOGE
#define APP_LOGW ESP_LOGW
#define APP_LOGI ESP_LOGI
#define APP_LOGD ESP_LOGD
#define APP_LOGV ESP_LOGV

#endif

