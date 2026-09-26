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
#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

// #ifdef __cplusplus
// extern "C"
// {
// #endif

// #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
// #include "esp_psram.h"
// #else
// #include "esp_spiram.h"

// #endif
// #ifdef __cplusplus
// }
// #endif



#include "./QueueMain.h"
#include "../AppContext.h"
#include "../AppVersion.h"
#include "../util/os.h"
#include "../util/chip_info.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "../AppLog.h"
static const char *TAG = STR(CLASSNAME);

////////////////////////////////////////////////////////////////////////////////////////////

#if defined ARDUPROF_FREERTOS
////////////////////////////////////////////////////////////////////////////////////////////
// QueueMain for Pi Pico/Pico2 (RP2040/RP2350) FreeRTOS
////////////////////////////////////////////////////////////////////////////////////////////
#define TASK_QUEUE_SIZE 16 // message queue size for app task
static uint8_t ucQueueStorageArea[TASK_QUEUE_SIZE * sizeof(Message)];
static StaticQueue_t xStaticQueue;

////////////////////////////////////////////////////////////////////////////////////////////
#elif defined ARDUPROF_MBED
////////////////////////////////////////////////////////////////////////////////////////////
// Thread for Pi Pico (RP2040) Mbed OS
////////////////////////////////////////////////////////////////////////////////////////////
#define THREAD_QUEUE_SIZE (128 * EVENTS_EVENT_SIZE) // message queue size for app thread

/////////////////////////////////////////////////////////////////////////////
// use static threadQueue instead of heap
static events::EventQueue threadQueue(THREAD_QUEUE_SIZE);

#endif

/////////////////////////////////////////////////////////////////////////////
CLASSNAME& CLASSNAME::getInstance(void)
{
    static CLASSNAME instance; // Guaranteed thread-safe initialization in C++ 11+
    return instance;
}

CLASSNAME::CLASSNAME() :
#if defined ARDUPROF_FREERTOS
                         ardufreertos::MessageBus(TASK_QUEUE_SIZE, ucQueueStorageArea, &xStaticQueue),
                         _handlerMap(),
                         _timer1Hz("Timer 1Hz",
                                   pdMS_TO_TICKS(1000),
                                   [](TimerHandle_t xTimer)
                                   {
                                        getInstance().postEvent(EventSystem, SysSoftwareTimer, 0, (uint32_t)xTimer);
                                   })
#elif defined ARDUPROF_MBED
                         ardumbedos::MessageBus(&threadQueue),
                         _handlerMap(),
                         _timer1Hz(queue(), 1000ms, [](int id)
                                   {
                                        getInstance().postEvent(EventSystem, SysSoftwareTimer, 0, (uint32_t)xTimer);
                                   })
#endif
{
    _handlerMap = {
        __EVENT_MAP(CLASSNAME, EventSystem),
        __EVENT_MAP(CLASSNAME, EventNull), // {EventNull, &CLASSNAME::handlerEventNull},
    };
}

void CLASSNAME::start(void *ctx)
{
    auto core = os::getRunningCore();
    auto prio = os::getPriority();
    auto freeHeapSize = os::getFreeHeapSize();
#if defined ARDUINO
    LOG_TRACE("core", core, ", priority=", prio, ", free heap size=", freeHeapSize);
#else
    APP_LOGD(TAG, "core %d, priority=%d, free heap size=%lu", core, prio, freeHeapSize);
#endif 

    MessageBus::start(ctx);

    chip::printInfo();

    _timer1Hz.start();
    // _timer1Hz.stop();

    // vTaskDelay(pdMS_TO_TICKS(1000));
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
        APP_LOGW(TAG, "Unsupported event=%d, iParam=%d, uParam=%d, lParam=%lu", msg.event, msg.iParam, msg.uParam, msg.lParam);
    }
}

/////////////////////////////////////////////////////////////////////////////
__EVENT_FUNC_DEFINITION(CLASSNAME, EventSystem, msg) // void CLASSNAME::handlerEventSystem(const Message &msg)
{
    // LOG_TRACE("EventSystem(", msg.event, "), iParam = ", msg.iParam, ", uParam = ", msg.uParam, ", lParam = ", msg.lParam);
    enum SystemTriggerSource src = static_cast<SystemTriggerSource>(msg.iParam);
    switch (src)
    {
    case SysSoftwareTimer:
        handlerSoftwareTimer((TimerHandle_t)(msg.lParam));
        break;
    // case SysButtonClick:
    //     handlerButtonClick(msg);
    //     break;
    default:
        APP_LOGW(TAG, "unsupported SystemTriggerSource=%d", src);
        break;
    }
}

// define EventNull handler
__EVENT_FUNC_DEFINITION(CLASSNAME, EventNull, msg) // void CLASSNAME::handlerEventNull(const Message &msg)
{
    APP_LOGD(TAG, "EventNull(%d), iParam=%d, uParam=%u, lParam=%lu", msg.event, msg.iParam, msg.uParam, msg.lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CLASSNAME::handlerSoftwareTimer(TimerHandle_t xTimer)
{
    if (xTimer == _timer1Hz.timer())
    {
        APP_LOGV(TAG, "_timer1Hz");
    }
    else
    {
        APP_LOGW(TAG, "unsupported timer handle=%p", xTimer);
    }
}
