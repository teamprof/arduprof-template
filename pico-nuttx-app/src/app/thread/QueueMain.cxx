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
#include <pthread.h>
#include <sched.h>
#include <syslog.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <nuttx/config.h>

#ifdef CONFIG_MM_REGIONS
#include <malloc.h>
#endif

#include "./QueueMain.h"
#include "../AppVersion.h"
#include "../peripheral/led_usr.h"


///////////////////////////////////////////////////////////////////////
#define TASK_QUEUE_LENGTH 16 // message queue length for app task

#define TASK_QUEUE_SIZE (TASK_QUEUE_LENGTH * SIZEOF_MESSAGE_POW2)
static uint8_t _queueBuffer[TASK_QUEUE_SIZE] __attribute__((aligned(4)));
// static void *_queuePointerArray[TASK_QUEUE_LENGTH];  
static void *_queuePointerArray[TASK_QUEUE_LENGTH] __attribute__((uninitialized));


///////////////////////////////////////////////////////////////////////
CLASSNAME *CLASSNAME::_instance = nullptr;

///////////////////////////////////////////////////////////////////////
CLASSNAME::CLASSNAME() : nuttxos::MessageBus(_queueBuffer, TASK_QUEUE_SIZE, _queuePointerArray, TASK_QUEUE_LENGTH),
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
                             }),
                         _ledState(false)
{
    _handlerMap = {
        __EVENT_MAP(CLASSNAME, EventSystem),
        __EVENT_MAP(CLASSNAME, EventNull),  // {EventNull, &QueueMain::handlerEventNull},
    };
}

CLASSNAME *QueueMain::getInstance(void)
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
    syslog(LOG_DEBUG, STR(CLASSNAME) "::start at core %d\n", sched_getcpu());
    MessageBus::start(ctx);

    printChipInfo();

    int ret = led_usr::init();
    // UNUSED(ret);
    if (ret)
    {
        syslog(LOG_ERR, STR(CLASSNAME) "::start: Failed to initialize led_usr: %d", ret);
    }

    syslog(LOG_DEBUG, STR(CLASSNAME) ": start: _timer1Hz.start()...\n");
    _timer1Hz.start();
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
        syslog(LOG_WARNING, STR(CLASSNAME) "::onMessage: Unsupported event=%hd, iParam=%hd, uParam=%hu, lParam=%lu",
               msg.event, msg.iParam, msg.uParam, msg.lParam);
    }
}

///////////////////////////////////////////////////////////////////////
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
    syslog(LOG_DEBUG, STR(CLASSNAME) ": EventNull(%hd), iParam=%hd, uParam=%hu, lParam=%lu", 
           msg.event, msg.iParam, msg.uParam, msg.lParam);
}

void CLASSNAME::handlerSoftwareTimer(timer_t timerid)
{
    if (timerid == _timer1Hz.timer())
    {
        syslog(LOG_DEBUG, STR(CLASSNAME) ": timer1Hz");
        toggleLedState();
    }
    else
    {
        syslog(LOG_WARNING, STR(CLASSNAME) ": unsupported timer=%p", timerid);
    }
}

///////////////////////////////////////////////////////////////////////
void CLASSNAME::printChipInfo(void)
{
// For RP2350 / RP2040 hardware architectures
#define RP_SYSINFO_BASE 0x40000000 // Standard SysInfo address space
#define RP_CHIP_ID_OFFSET 0x00000000
#define RP_CHIP_ID_REG (*(volatile uint32_t *)(RP_SYSINFO_BASE + RP_CHIP_ID_OFFSET))

    uint32_t chip_id = RP_CHIP_ID_REG;

    // The top 4 bits (31:28) typically hold the silicon revision stepping
    uint8_t revision = (chip_id >> 28) & 0x0F;

    syslog(LOG_INFO, "===============================================================================\n");

    struct utsname os_info;
    if (uname(&os_info) == 0)
    {
        syslog(LOG_INFO, "OS Name:       %s\n", os_info.sysname);
        syslog(LOG_INFO, "Release (Ver): %s\n", os_info.release);
        syslog(LOG_INFO, "Build Version: %s\n", os_info.version);
        syslog(LOG_INFO, "Arch/Chip:     %s\n", os_info.machine);
    }
    else
    {
        syslog(LOG_WARNING, STR(CLASSNAME) "::printChipInfo: Failed to retrieve OS layout definitions.\n");
    }

    syslog(LOG_INFO, "MCU:           %s, rev=%s, chip ID=0x%08X\n",
           CONFIG_ARCH_CHIP,
           (revision == 1) ? "B0" : "A1/A2",
           (unsigned int)chip_id);
    syslog(LOG_INFO, "Board:         %s\n", CONFIG_ARCH_BOARD);

#ifdef CONFIG_SMP
    syslog(LOG_INFO, "SMP Status:    Enabled, Cores=%d\n", CONFIG_SMP_NCPUS);
#else
    syslog(LOG_INFO, "SMP Status:    Disabled (Single-Core Mode)\n");
#endif

// Read the execution core running this exact line of code
#ifdef CONFIG_SMP
    syslog(LOG_INFO, "Current Core:  Executing on CPU %d\n", sched_getcpu());
#endif

    struct sysinfo mem_info;
    if (sysinfo(&mem_info) == 0)
    {
        syslog(LOG_INFO, "Total RAM:     %lu bytes\n", (unsigned long)mem_info.totalram);
        syslog(LOG_INFO, "Free RAM:      %lu bytes\n", (unsigned long)mem_info.freeram);
        syslog(LOG_INFO, "Buffer RAM:    %lu bytes\n", (unsigned long)mem_info.bufferram);
    }

    // 4. Detailed Heap Contiguity Breakdown (If allocator metrics are enabled)
#ifdef CONFIG_MM_REGIONS
    struct mallinfo mem_details = mallinfo();
    syslog(LOG_INFO, "Max Alloc Block: %d bytes (Largest continuous chunk)\n", mem_details.mxordblk);
    syslog(LOG_INFO, "Allocated Heap:  %d bytes (In-use memory)\n", mem_details.uordblks);
#endif

    syslog(LOG_INFO, "===============================================================================\n");
}

///////////////////////////////////////////////////////////////////////
bool CLASSNAME::getLedState(void) { return _ledState; }
void CLASSNAME::setLedState(bool ledState)
{
    _ledState = ledState;
    // syslog(LOG_DEBUG, STR(CLASSNAME) "_ledState=%d", (int)_ledState);

    auto ret = led_usr::set(_ledState);
    UNUSED(ret);
    // (void)ret;
    // if (ret < 0)
    // {
    //     syslog(LOG_ERR, STR(CLASSNAME) "gpio_pin_set_dt() returns %d", ret);
    // }
}

void CLASSNAME::toggleLedState(void) { setLedState(!_ledState); }
