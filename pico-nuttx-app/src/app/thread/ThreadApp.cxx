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
#include <sched.h>
#include <syslog.h>

#include "./ThreadApp.h"
#include <unistd.h>

#include <nuttx/config.h>
#include <nuttx/init.h>
#include <nuttx/board.h>
#include <nuttx/clock.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////
// #define CORE (-1) // let OS to assign running core
#define CORE 0 // running on core 0 (-1, 0, or 1)
// #define CORE 1 // running on core 1 (-1, 0, or 1)

#define TASK_STACK_SIZE 2048
#define TASK_PRIORITY 8         // Lower Number = Higher Priority, number is 1 ~ 255
#define TASK_QUEUE_LENGTH 16    // message queue length for app task
static_assert(TASK_PRIORITY > SCHED_PRIORITY_MIN, "TASK_PRIORITY exceeds SCHED_PRIORITY_MAX");
static_assert(TASK_PRIORITY <= SCHED_PRIORITY_MAX, "TASK_PRIORITY exceeds SCHED_PRIORITY_MAX");

///////////////////////////////////////////////////////////////////////
#define TASK_QUEUE_SIZE (TASK_QUEUE_LENGTH * SIZEOF_MESSAGE_POW2)
static uint8_t _queueBuffer[TASK_QUEUE_SIZE] __attribute__((aligned(4)));
static void *_queuePointerArray[TASK_QUEUE_LENGTH];  

///////////////////////////////////////////////////////////////////////
CLASSNAME *CLASSNAME::_instance = NULL;

///////////////////////////////////////////////////////////////////////
CLASSNAME::CLASSNAME() : nuttxos::ThreadBase(CORE, TASK_STACK_SIZE, TASK_PRIORITY,
                                             _queueBuffer, TASK_QUEUE_SIZE, _queuePointerArray, TASK_QUEUE_LENGTH),
                         _timer1Hz(
                             {
                                 .it_interval = {
                                     .tv_sec = 1,
                                     .tv_nsec = 0,
                                 },
                                 .it_value = {
                                     .tv_sec = 1,
                                     .tv_nsec = 0,
                                 },

                             },
                             [](timer_t timer_id)
                             {
                                 auto instance = CLASSNAME::getInstance();
                                 instance->postEvent(EventSystem, SysSoftwareTimer, 0, (uint32_t)(timer_id));
                                 //
                             })
{
    _handlerMap = {
        __EVENT_MAP(CLASSNAME, EventSystem),
        __EVENT_MAP(CLASSNAME, EventNull),  // {EventNull, &ThreadSlave::handlerEventNull},
    };
}

CLASSNAME *CLASSNAME::getInstance(void)
{
    if (!_instance)
    {
        static CLASSNAME instance;
        _instance = &instance;
    }
    return _instance;
}

void CLASSNAME::start(void *ctx)
{
    ThreadBase::start(ctx);
}

void CLASSNAME::setup(void)
{
    syslog(LOG_DEBUG, STR(CLASSNAME) "::setup() at core %d\n", sched_getcpu());

    _timer1Hz.start();
    // _timer1Hz.stop();
}

void CLASSNAME::onMessage(const Message &msg)
{
    auto func = _handlerMap[msg.event];
    if (func)
    {
        (this->*func)(msg);
    }
    else
    {
        syslog(LOG_WARNING, STR(CLASSNAME) ": unsupported event=%hd, iParam=%hd, uParam=%hu, lParam=%lu",
               msg.event, msg.iParam, msg.uParam, msg.lParam);
    }
}

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// void CLASSNAME::handlerEventSystem(const Message &msg)
__EVENT_FUNC_DEFINITION(CLASSNAME, EventSystem, msg)
{
    syslog(LOG_DEBUG, STR(CLASSNAME) ": EventSystem(%hd), iParam=%hd, uParam=%hu, lParam=0x%08lx",
           msg.event, msg.iParam, msg.uParam, msg.lParam);
    enum SystemTriggerSource src =
        static_cast<enum SystemTriggerSource>(msg.iParam);
    switch (src)
    {
    case SysSoftwareTimer:
        handlerSoftwareTimer((timer_t)(msg.lParam));
        break;
    default:
        syslog(LOG_WARNING, STR(CLASSNAME) ": unsupported SystemTriggerSource=%hd", src);
        break;
    }
}
// void CLASSNAME::handlerEventNull(const Message &msg)
__EVENT_FUNC_DEFINITION(CLASSNAME, EventNull, msg)
{
    syslog(LOG_DEBUG, STR(CLASSNAME) ": EventNull(%hd), iParam=%hd, uParam=%hu, lParam=%lu", msg.event,
           msg.iParam, msg.uParam, msg.lParam);
}

///////////////////////////////////////////////////////////////////////
void CLASSNAME::handlerSoftwareTimer(timer_t timerid)
{
    if (timerid == _timer1Hz.timer())
    {
        syslog(LOG_DEBUG, STR(CLASSNAME) ": timer1Hz");
    }
    else
    {
        syslog(LOG_WARNING, STR(CLASSNAME) ": unsupported timer=%p", timerid);
    }
}
