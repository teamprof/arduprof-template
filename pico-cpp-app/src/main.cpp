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
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h" 

#include "./ArduProfApp.h"
#include "./AppContext.h"
// #include "./thread/QueueMain.h"
#include "./peripheral/unused_pins.h"

#define LOG_LOCAL_LEVEL APP_LOG_VERBOSE
#include "./AppLog.h"
static const char *TAG = STR(main);

#ifdef PICO_RP2040 
#pragma message("PICO_RP2040")
#endif

#ifdef PICO_RP2350
#pragma message("PICO_RP2350")
#endif


static void initGlobalVar(void)
{
}

static void startTasks(void)
{
    APP_LOGI(TAG, "startTasks...");

    auto& ctx = getAppContext();
    if (ctx.queueMain) {
        // static_cast<QueueMain *>(ctx.queueMain)->start(&ctx);
        ctx.queueMain->start(&ctx);
    } else {
        APP_LOGW(TAG, "ctx->queueMain is NULL");
    }
    if (ctx.threadApp) {
        ctx.threadApp->start(&ctx);
    } else {
        APP_LOGW(TAG, "ctx.threadApp is NULL");
    }
    // if (ctx.threadButton) {
    //     ctx.threadButton->start(&ctx);
    // } else {
    //     // APP_LOGW(TAG, "ctx.threadButton is NULL");
    // }
}


static void setup(void)
{
    stdio_init_all();

    const char *sDebugPort = "Hardware UART0 (Default)";
#if LIB_PICO_STDIO_USB
    #define WAIT_USB_TIMEOUT 30    // in units of 0.1s
    bool isUsbConnected;
    for(int i=0; i<WAIT_USB_TIMEOUT; i++)
    {
        isUsbConnected = stdio_usb_connected();
        if(isUsbConnected)
        {
            sDebugPort = "USB CDC (Virtual COM)";
            break;
        }
        sleep_ms(100);
    }
#endif
    APP_LOGI(TAG, "debug port is %s", sDebugPort);

    unused_pins::setup();

    initGlobalVar();
    startTasks();
    // sleep_ms(2000);
}

// loop is not be invoked as main is not running in thread/task
static void loop(void)
{
    // auto& ctx = getAppContext();
    // auto qMain = static_cast<QueueMain *>(ctx.queueMain);
    // if(qMain)
    // {
    //     // qMain->messageLoop(0); // non-blocking
    //     // qMain->messageLoop();  // blocking until event received and proceed
    //     qMain->messageLoopForever(); // never return
    // }
    // vTaskDelay(pdMS_TO_TICKS(100));

    static uint32_t count = 0;
    // printf("count: %lu\n", count++);
    APP_LOGD(TAG, "Count: %lu", count++);
    // vTaskDelay(pdMS_TO_TICKS(1000));
    sleep_ms(1000);
}

int main()
{
    setup();
    vTaskStartScheduler();

    APP_LOGW(TAG, "Should NOT be here!");
    while (true) {
        loop();
        sleep_ms(1000);
        // vTaskDelay(pdMS_TO_TICKS(100));
    }
}
