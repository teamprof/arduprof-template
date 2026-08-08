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
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>

#include "AppContext.h"
#include "AppEvent.h"
#include "thread/QueueMain.h"

#ifdef __cplusplus
extern "C" int main(int argc, FAR char *argv[]);
#endif

int main(int argc, FAR char *argv[])
{
    // // Only allow warnings and higher (suppresses LOG_INFO and LOG_DEBUG)
    // setlogmask(LOG_UPTO(LOG_WARNING));

    // Allow everything up to and including debug
    setlogmask(LOG_UPTO(LOG_DEBUG));


    auto ctx = get_context();
    if (ctx->threadApp)
    {
        ctx->threadApp->start(ctx);
    }
    else
    {
        syslog(LOG_ERR, "No threadApp in context");
    }

    if (ctx->queueMain)
    {
        auto queueMain = static_cast<QueueMain *>(ctx->queueMain);
        queueMain->start(ctx);
        queueMain->postEvent(EventNull);
        // queueMain->messageLoop(MSEC2TICK(0));
        queueMain->messageLoopForever();
    }
    else
    {
        syslog(LOG_ERR, "No queueMain in context");
    }

    return EXIT_SUCCESS;
}
