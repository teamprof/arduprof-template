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

// "rpi_pico",
// "-DDTC_OVERLAY_FILE=boards/rpi_pico2_rp2040_m0+.overlay",

// "rpi_pico2/rp2350a/m33",
// "-DDTC_OVERLAY_FILE=boards/rpi_pico2_rp2350a_m33.overlay",

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "AppContext.h"
#include "AppEvent.h"
#include "thread/QueueMain.h"

// LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

///////////////////////////////////////////////////////////////////////
int main(void)
{
    LOG_INF("...");
    auto ctx = get_app_context();
    if (ctx->threadApp)
    {
        ctx->threadApp->start(ctx);
    }
    else
    {
        LOG_ERR("No threadApp in context");
    }

    if (ctx->queueMain)
    {
        auto queueMain = static_cast<QueueMain *>(ctx->queueMain);
        queueMain->start(ctx);
        queueMain->postEvent(EventNull);
        queueMain->messageLoopForever();
    }
    else
    {
        LOG_ERR("No queueMain in context");
    }

    return 0;
}

// LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

// int main(void)
// {
//     printk("Hello World from Pico!\n");

//     while (1)
//     {
//         printk("Running on %s...\n", CONFIG_BOARD);

//         k_sleep(K_MSEC(1000));
//     }

//     return 0;
// }
