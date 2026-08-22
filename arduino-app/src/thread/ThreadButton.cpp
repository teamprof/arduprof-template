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
#include "./ThreadButton.h"
#include "../AppContext.h"

////////////////////////////////////////////////////////////////////////////////////////////

#if defined ARDUPROF_FREERTOS

#if defined ARDUINO_ARCH_RP2040
////////////////////////////////////////////////////////////////////////////////////////////
// Thread for Pi Pico/Pico2 (RP2040/RP2350) FreeRTOS
////////////////////////////////////////////////////////////////////////////////////////////
static constexpr UBaseType_t uxCoreAffinityMask = ((1 << 0));           // task only run on core 0
// static constexpr UBaseType_t uxCoreAffinityMask = ((1 << 1));        // task only run on core 1 
// static constexpr uxCoreAffinityMask = ( ( 1 << 0 ) | ( 1 << 2 ) );   // e.g. task can only run on core 0 and core 2

#elif defined ESP_PLATFORM
////////////////////////////////////////////////////////////////////////////////////////////
// Thread for ESP32
////////////////////////////////////////////////////////////////////////////////////////////
#define RUNNING_CORE ARDUINO_RUNNING_CORE
// #define RUNNING_CORE 0 // dedicate core 0 for Thread
// #define RUNNING_CORE 1 // dedicate core 1 for Thread

#endif


#define TASK_NAME STR(CLASSNAME)
#define TASK_STACK_SIZE (1024 / sizeof(StackType_t))
#define TASK_PRIORITY 12    // Priority, (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
#define TASK_QUEUE_SIZE 8   // message queue size for app task
static_assert(TASK_PRIORITY <= configMAX_PRIORITIES, "TASK_PRIORITY exceeds configMAX_PRIORITIES");

#define TASK_INIT_NAME "taskDelayInit"
#define TASK_INIT_STACK_SIZE (4096 / sizeof(StackType_t))
#define TASK_INIT_PRIORITY 0
static_assert(TASK_INIT_PRIORITY <= configMAX_PRIORITIES, "TASK_INIT_PRIORITY exceeds configMAX_PRIORITIES");

static uint8_t ucQueueStorageArea[TASK_QUEUE_SIZE * sizeof(Message)];
static StaticQueue_t xStaticQueue;

static StackType_t xStack[TASK_STACK_SIZE];
static StaticTask_t xTaskBuffer;

///////////////////////////////////////////////////////////////////////
void CLASSNAME::start(void *ctx) 
{
#if defined ARDUINO_ARCH_RP2040
    LOG_TRACE("core", get_core_num(), ", uxTaskPriorityGet(NULL)=", uxTaskPriorityGet(NULL));
#elif defined ARDUINO_ARCH_ESP32
    LOG_TRACE("on core ", xPortGetCoreID(), ", xPortGetFreeHeapSize()=", xPortGetFreeHeapSize());
#else    
    LOG_TRACE("");
#endif

    configASSERT(ctx);
    _context = ctx;

#if defined ARDUINO_ARCH_RP2040
    _taskHandle = xTaskCreateStatic(
        [](void *instance) { static_cast<ThreadBase *>(instance)->run(); },
        TASK_NAME,
        TASK_STACK_SIZE,    // This stack size can be checked & adjusted by reading the Stack Highwater
        this,
        TASK_PRIORITY,      // Priority, (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        xStack, &xTaskBuffer);
    configASSERT(_taskHandle);
    vTaskCoreAffinitySet(_taskHandle, uxCoreAffinityMask); // Set the core affinity mask for the task, i.e. set task on running core

#elif defined ESP_PLATFORM
    _taskHandle = xTaskCreateStaticPinnedToCore(
        [](void *instance) { static_cast<ThreadBase *>(instance)->run(); },
        TASK_NAME,
        TASK_STACK_SIZE,    // This stack size can be checked & adjusted by reading the Stack Highwater
        this,
        TASK_PRIORITY,      // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        xStack, &xTaskBuffer, RUNNING_CORE);

#endif
}

#elif defined ARDUPROF_MBED && defined ARDUINO_ARCH_MBED_RP2040
////////////////////////////////////////////////////////////////////////////////////////////
// Thread for Pi Pico (RP2040) Mbed OS
////////////////////////////////////////////////////////////////////////////////////////////
#define THREAD_QUEUE_SIZE (64 * EVENTS_EVENT_SIZE) // message queue size for app thread
#include "../util/util.h"
/////////////////////////////////////////////////////////////////////////////
// use static threadQueue instead of heap
static events::EventQueue threadQueue(THREAD_QUEUE_SIZE);

/////////////////////////////////////////////////////////////////////////////
void CLASSNAME::start(void *ctx) 
{
    LOG_TRACE("core", get_core_num(), ", ctx=(hex)", DebugLogBase::HEX, (uint32_t)ctx);
    ThreadBase::start(ctx);
}

#endif

/////////////////////////////////////////////////////////////////////////////
CLASSNAME::CLASSNAME() :
#if defined ARDUPROF_FREERTOS
      ardufreertos::ThreadBase(TASK_QUEUE_SIZE, ucQueueStorageArea, &xStaticQueue),
#if defined ARDUINO_ARCH_RP2040      
      _timer1Hz("Timer 1Hz", pdMS_TO_TICKS(1000),
                [](TimerHandle_t xTimer) {
                    getInstance().postEvent(EventSystem, SysSoftwareTimer, 0, (uint32_t)xTimer);
                }),
#endif                
#elif defined ARDUPROF_MBED && defined ARDUINO_ARCH_MBED_RP2040
      ardumbedos::ThreadBase(&threadQueue),
      //  ardumbedos::ThreadBase(THREAD_QUEUE_SIZE),  // threadQueue is dynamically allocate from heap
      _timer1Hz(queue(), 1000ms,
                [](int id) {
                    getInstance().postEvent(EventSystem, SysSoftwareTimer, 0, (uint32_t)xTimer);
                }),
#endif
       _buttonBoot(queue()),
       _debounceTimer(queue(), EventSystem, SysSoftwareTimer),
      _handlerMap() 
{
  _handlerMap = {
      __EVENT_MAP(CLASSNAME, EventSystem),
      __EVENT_MAP(CLASSNAME, EventGpioISR),
      __EVENT_MAP(CLASSNAME, EventNull), // {EventNull, &CLASSNAME::handlerEventNull},
  };
}

void CLASSNAME::setup(void) 
{
#if defined ARDUPROF_FREERTOS && defined ARDUINO_ARCH_RP2040
    LOG_TRACE("core", get_core_num(), ", uxTaskPriorityGet(NULL)=", uxTaskPriorityGet(NULL));
#endif

    ThreadBase::setup();

    _buttonBoot.init(EventSystem, SysButtonClick, SysButtonDoubleClick, SysButtonLongPress); 
    _debounceTimer.attachButton(&_buttonBoot);

    // vTaskDelay(pdMS_TO_TICKS(1000));
}

/////////////////////////////////////////////////////////////////////////////
void CLASSNAME::onMessage(const Message &msg) {
    // LOG_TRACE("event=", msg.event, ", iParam=", msg.iParam, ", uParam=", msg.uParam, ", lParam=", msg.lParam);
    auto func = _handlerMap[msg.event];
    if (func) {
        (this->*func)(msg);
    } else {
        LOG_TRACE("Unsupported event=", msg.event, ", iParam=", msg.iParam, ", uParam=", msg.uParam, ", lParam=", msg.lParam);
    }
}

/////////////////////////////////////////////////////////////////////////////
__EVENT_FUNC_DEFINITION(CLASSNAME, EventGpioISR, msg) // void CLASSNAME::handlerEventGpioISR(const Message &msg)
{
    // LOG_TRACE("EventGpioISR(", msg.event, "), iParam = ", msg.iParam, ", uParam = ", msg.uParam, ", lParam = ", msg.lParam);

    uint8_t pin = msg.iParam;
    uint8_t value = msg.uParam;
    if (pin == _buttonBoot.getPin())
    {
        uint16_t clickCount = _buttonBoot.getClickCount();
        // LOG_TRACE("EventGpioISR: _buttonBoot: clickCount=", clickCount);
        uint32_t ms = msg.lParam;
        _buttonBoot.onEventIsr(value, ms);
    }
    else
    {
        LOG_TRACE("unsupported button: GPIO", pin);
    }
}

__EVENT_FUNC_DEFINITION(CLASSNAME, EventSystem, msg) // void CLASSNAME::handlerEventSystem(const Message &msg)
{
    // LOG_TRACE("EventSystem(", msg.event, "), iParam = ", msg.iParam, ", uParam = ", msg.uParam, ", lParam = ", msg.lParam);
    enum SystemTriggerSource src = static_cast<SystemTriggerSource>(msg.iParam);
    switch (src) {
    case SysSoftwareTimer:
        handlerSoftwareTimer((TimerHandle_t)(msg.lParam));
        break;
    case SysButtonClick:
    {
        handlerButtonClick(msg);
        break;
    }
    case SysButtonDoubleClick:
    {
        handlerButtonDoubleClick(msg);
        break;
    }
    case SysButtonLongPress:
    {
        handlerButtonLongPress(msg);
        break;
    }
    default:
        LOG_TRACE("unsupported SystemTriggerSource=", src);
        break;
    }
}

// define EventNull handler
__EVENT_FUNC_DEFINITION(CLASSNAME, EventNull, msg) // void CLASSNAME::handlerEventNull(const Message &msg)
{
    LOG_TRACE("EventNull(", msg.event, "), iParam=", msg.iParam, ", uParam=", msg.uParam, ", lParam=", msg.lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CLASSNAME::handlerSoftwareTimer(TimerHandle_t xTimer) {
    if (xTimer == _debounceTimer.timer())
    {
        // LOG_TRACE("_debounceTimer::timer()");
        _debounceTimer.onEventTimer();
    }
#if defined ARDUINO_ARCH_RP2040
    else if (xTimer == _timer1Hz.timer()) 
    {
        LOG_TRACE("_timer1Hz");

        extern bool __no_inline_not_in_flash_func(get_bootsel_button)();
        static bool lastState = false;
        bool state = get_bootsel_button();
        if (lastState != state) {
            LOG_TRACE("BOOTSEL button state changed: ", state);
            lastState = state;
        }
    } 
#endif
    else 
    {
      LOG_TRACE("unsupported timer handle=0x%04x", (uint32_t)(xTimer));
    }
}

void CLASSNAME::handlerButtonClick(const Message &msg)
{
    int16_t pin = msg.uParam;
    // LOG_TRACE("pin =", pin);
    if (pin == _buttonBoot.getPin())
    {
        LOG_TRACE("ButtonClick: _buttonBoot");
        auto ctx = reinterpret_cast<AppContext *>(context());
        auto dst = ctx->queueMain;
        postEvent(dst, EventApp, AppButton, SysButtonClick, pin);
    }
    else
    {
        LOG_TRACE("SysButtonClick: unsupported pin=", pin);
    }
}
void CLASSNAME::handlerButtonDoubleClick(const Message &msg)
{
    int16_t pin = msg.uParam;
    if (pin == _buttonBoot.getPin())
    {
        LOG_TRACE("SysButtonDoubleClick: _buttonBoot");
        auto ctx = reinterpret_cast<AppContext *>(context());
        auto dst = ctx->queueMain;
        postEvent(dst, EventApp, AppButton, SysButtonDoubleClick, pin);
    }
    else
    {
        LOG_TRACE("SysButtonDoubleClick: unsupported pin=", pin);
    }
}
void CLASSNAME::handlerButtonLongPress(const Message &msg)
{
    int16_t pin = msg.uParam;
    if (pin == _buttonBoot.getPin())
    {
        LOG_TRACE("SysButtonLongPress: _buttonBoot");
        auto ctx = reinterpret_cast<AppContext *>(context());
        auto dst = ctx->queueMain;
        postEvent(dst, EventApp, AppButton, SysButtonLongPress, pin);
    }
    else
    {
        LOG_TRACE("SysButtonLongPress: unsupported pin=", pin);
    }
}

/////////////////////////////////////////////////////////////////////////////
