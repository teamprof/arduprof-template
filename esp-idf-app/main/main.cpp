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

// # UART0 as debug log port
// CONFIG_ESP_CONSOLE_UART_DEFAULT=y

// # USB CDC as debug log port
// CONFIG_ESP_CONSOLE_UART_DEFAULT=n
// # CONFIG_ESP_CONSOLE_USB_CDC=y
// CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y


// #include <stdio.h>
#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
// #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "./AppLog.h"
// #include "esp_log.h"
static const char *TAG = "main";

#include "./ArduProfApp.h"
#include "./AppContext.h"
#include "./thread/QueueMain.h"
#include "./peripheral/unused_pins.h"


#ifdef __cplusplus
extern "C" void app_main(void);
#endif

static void initGlobalVar(void)
{
}

static void startTasks(void)
{
    APP_LOGV(TAG, "startTasks...");

    auto& ctx = getAppContext();
    if (ctx.queueMain) {
        static_cast<QueueMain *>(ctx.queueMain)->start(&ctx);
    } else {
        APP_LOGW(TAG, "ctx->queueMain is NULL");
    }
    if (ctx.threadApp) {
        ctx.threadApp->start(&ctx);
    } else {
        APP_LOGW(TAG, "ctx.threadApp is NULL");
    }
    if (ctx.threadButton) {
        ctx.threadButton->start(&ctx);
    } else {
        APP_LOGW(TAG, "ctx.threadButton is NULL");
    }
}

static void setup(void)
{
    esp_log_level_set("*", ESP_LOG_VERBOSE);

#if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT)    
    APP_LOGI(TAG, "debug port is Hardware UART0 (Default)");
#elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    vTaskDelay(pdMS_TO_TICKS(1000));
    APP_LOGI(TAG, "debug port is Internal USB Serial/JTAG Controller");
    vTaskDelay(pdMS_TO_TICKS(1000));
#elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
    vTaskDelay(pdMS_TO_TICKS(1000));
    APP_LOGI(TAG, "debug port is USB CDC (Virtual COM via TinyUSB)");
#endif

    unused_pins::setup();

    initGlobalVar();
    startTasks();
}

static void loop(void)
{
    auto& ctx = getAppContext();
    auto qMain = static_cast<QueueMain *>(ctx.queueMain);
    if(qMain)
    {
        // qMain->messageLoop(0); // non-blocking
        // qMain->messageLoop();  // blocking until event received and proceed
        qMain->messageLoopForever(); // never return
    }
    vTaskDelay(pdMS_TO_TICKS(100));

    static uint32_t count = 0;
    APP_LOGI(TAG, "Count: %lu", count++);
    vTaskDelay(pdMS_TO_TICKS(1000));
}

///////////////////////////////////////////////////////////////////////////////
void app_main(void)
{
    setup();
    while (true) {
        loop();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}