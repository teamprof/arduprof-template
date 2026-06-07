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
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/types.h>
#include <zephyr/sys/mem_stats.h>

#include <zephyr/sys/util.h>

// #include <zephyr/drivers/hwinfo.h>
// #include <soc.h>

#if defined(CONFIG_SOC_RP2040)
// RP2040 (Cortex-M0+)
#include "hardware/regs/m0plus.h"
#elif defined(CONFIG_SOC_RP2350)
// RP2350 / M33 registers
#include "hardware/regs/m33.h"
#else
#error "Unknown or unsupported SoC target!"
#endif

#include "hardware/regs/addressmap.h"

// // RP2350 specific headers from the HAL
// #include "hardware/regs/addressmap.h"
// // #include "hardware/rp2040_registers.h"
// #include "hardware/structs/psm.h"
// #include "hardware/regs/addressmap.h"

// #include "hardware/structs/syscfg.h"
// #include "pico/platform.h"

// // #include "hardware/regs/m0plus.h" // If you need CPUID on M0+
// // // OR for the RP2350 (Cortex-M33)
// // #include "hardware/structs/syscfg.h"
// // #include "pico/platform.h"

#include "./QueueMain.h"
#include "../AppVersion.h"
#include "../peripheral/led_usr.h"

// LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);
LOG_MODULE_REGISTER(QueueMain, LOG_LEVEL_DBG);

///////////////////////////////////////////////////////////////////////
// K_TIMER_DEFINE(QueueMain::_timer1Hz, QueueMain::_timerExpiryHandler,
//                QueueMain::_timerStopHandler);

///////////////////////////////////////////////////////////////////////
#define TASK_QUEUE_SIZE 16 // message queue size for app task
#define QUEUENAME CONCAT(QueueMain, taskQueue)

///////////////////////////////////////////////////////////////////////
K_MSGQ_DEFINE(QUEUENAME, sizeof(Message), TASK_QUEUE_SIZE, alignof(uint32_t));

QueueMain *QueueMain::_instance = NULL;

///////////////////////////////////////////////////////////////////////
// QueueMain::QueueMain() : zephyros::MessageBus(&QUEUENAME)
QueueMain::QueueMain() : zephyros::MessageBus(&QUEUENAME), _ledState(false)
{
    _handlerMap = {
        __EVENT_MAP(QueueMain, EventSystem),

        // {EventNull, &QueueMain::handlerEventNull},
        __EVENT_MAP(QueueMain, EventNull),
    };
}

QueueMain *QueueMain::getInstance(void)
{
    if (!_instance)
    {
        static QueueMain instance;
        _instance = &instance;
    }
    return _instance;
}

void QueueMain::start(void *ctx)
{
    MessageBus::start(ctx);

    printChipInfo();

    int ret = led_usr::init();
    if (ret != 0)
    {
        LOG_ERR("Failed to initialize led_usr: %d", ret);
    }

    k_timer_init(&_timer1Hz, QueueMain::_timerExpiryHandler, QueueMain::_timerStopHandler);
    k_timer_start(&_timer1Hz, K_MSEC(1000), K_SECONDS(1));
}

void QueueMain::onMessage(const Message &msg)
{
    auto func = _handlerMap[msg.event];
    if (func)
    {
        (this->*func)(msg);
    }
    else
    {
        LOG_DBG("Unsupported event=%hd, iParam=%hd, uParam=%hu, lParam=%u",
                msg.event, msg.iParam, msg.uParam, msg.lParam);
    }
}

///////////////////////////////////////////////////////////////////////
bool QueueMain::getLedState(void) { return _ledState; }
void QueueMain::setLedState(bool ledState)
{
    _ledState = ledState;
    LOG_DBG("_ledState=%d", (int)_ledState);

    auto ret = led_usr::set(_ledState);
    if (ret < 0)
    {
        LOG_ERR("gpio_pin_set_dt() returns %d", ret);
    }
}

void QueueMain::toggleLedState(void) { setLedState(!_ledState); }

///////////////////////////////////////////////////////////////////////

// void QueueMain::handlerEventSystem(const Message &msg)
__EVENT_FUNC_DEFINITION(QueueMain, EventSystem, msg)
{
    // LOG_DBG("EventSystem(%hd), iParam=%hd, uParam=%hu, lParam=0x%08x",
    // msg.event, msg.iParam, msg.uParam, msg.lParam);
    enum SystemTriggerSource src =
        static_cast<enum SystemTriggerSource>(msg.iParam);
    switch (src)
    {
    case SysSoftwareTimer:
        handlerSoftwareTimer((k_timer *)(msg.lParam));
        break;
    default:
        LOG_DBG("unsupported SystemTriggerSource=%hd", src);
        break;
    }
}
// void QueueMain::handlerEventNull(const Message &msg)
__EVENT_FUNC_DEFINITION(QueueMain, EventNull, msg)
{
    LOG_DBG("EventNull(%hd), iParam=%hd, uParam=%hu, lParam=%u", msg.event,
            msg.iParam, msg.uParam, msg.lParam);
}

void QueueMain::handlerSoftwareTimer(k_timer *timer)
{
    if (timer == &_timer1Hz)
    {
        LOG_DBG("timer1Hz");
        // toggleLedState();
    }
    else
    {
        LOG_DBG("unsupported timer=%p", timer);
    }
}

///////////////////////////////////////////////////////////////////////
void QueueMain::printChipInfo(void)
{
    // #if defined(CONFIG_SOC_RP2040)
    // // Code specific to RP2040 (Cortex-M0+)
    // #define CHIP_NAME "RP2040"
    // #elif defined(CONFIG_SOC_RP2350)
    // Code specific to RP2350 (Cortex-M33)
    // Chip and ROM Version
    // The version is stored in the last 4 bits of the CPUID register
    uint32_t cpu_id = *((volatile uint32_t *)(PPB_BASE + 0xED00));
    uint8_t chip_rev = (cpu_id >> 20) & 0xF;

    // ROM version is stored at a fixed address in the bootrom
    uint8_t rom_ver = *((uint8_t *)0x00000013);

    // const char *soc_name = CONFIG_SOC;
    // char soc_name[32];
    // ssize_t ret = soc_get_name(soc_name, sizeof(soc_name));

    LOG_INF("===============================================================================");
    LOG_INF("App Firmware version=%s", AppVersion::getFirmwareVersionString());
    LOG_INF("Chip Revision: %s %c", CONFIG_SOC, (chip_rev == 0) ? 'A' : (chip_rev + 'A'));
    LOG_INF("ROM Version:   %d", rom_ver);

    //     // PSRAM Size
    //     // On RP2350, PSRAM is typically initialized via the bootloader/Pico SDK.
    //     // If enabled, it is mapped to 0x11000000.
    // #if defined(CONFIG_RP2_FLASH_W25Q080) || defined(PICO_PSRAM_SIZE_BYTES)
    //     // Note: This often requires checking specific board definitions
    //     LOG_INF("PSRAM Size:    %d MB", (PICO_PSRAM_SIZE_BYTES / (1024 * 1024)));
    // #else
    //     LOG_INF("PSRAM Size:    Not detected or not enabled");
    // #endif
    // #else
    // #error "Unknown or unsupported SoC target!"
    // #endif

    // CONFIG_HEAP_MEM_POOL_SIZE=16384
    // CONFIG_SYS_HEAP_RUNTIME_STATS=y

    //     // Heap Statistics
    //     struct sys_memory_stats stats;

    //     extern struct k_heap _system_heap;

    //     // Get stats for the default system heap
    //     int err = sys_heap_runtime_stats_get(&_system_heap.heap, &stats);
    //     // sys_heap_runtime_stats_get(k_get_system_heap(), &stats);
    //     if (err == 0)
    //     {
    //         LOG_INF("Total Heap:    %zu bytes", stats.max_allocated_bytes + stats.free_bytes);
    //         LOG_INF("Free Heap:     %zu bytes", stats.free_bytes);
    //         LOG_INF("Used Heap:     %zu bytes", stats.allocated_bytes);
    //     }
    //     else
    //     {
    //         LOG_WRN("Failed to get heap stats (%d)\n", err);
    //     }

    // #if defined ARDUINO_ARCH_RP2040
    //     PRINTLN("rp2040_chip_version():", rp2040_chip_version(), ", rp2040_rom_version():", rp2040_rom_version(),
    //             "\r\nPSRAM total size:", rp2040.getPSRAMSize(),
    //             "\r\ntotal heap:", rp2040.getTotalHeap(), ", free heap:", rp2040.getFreeHeap());

    // #elif defined ESP_PLATFORM
    //     PRINTLN("ESP.getChipModel()=", ESP.getChipModel(), ", getChipRevision()=", ESP.getChipRevision(), ", getFlashChipSize()=", ESP.getFlashChipSize(),
    //             "\r\nNumber of cores=", ESP.getChipCores(), ", SDK version=", ESP.getSdkVersion(),
    //             "\r\nPSRAM total size=", ESP.getPsramSize(), " bytes, PSRAM free size=", ESP.getFreePsram(), " bytes");
    // #endif
    LOG_INF("===============================================================================");
}

void QueueMain::_timerExpiryHandler(struct k_timer *timer)
{
    auto instance = QueueMain::getInstance();
    // QueueMain *instance = CONTAINER_OF(timer, QueueMain, _timer1Hz);
    instance->postEvent(EventSystem, SysSoftwareTimer, 0, (uint32_t)timer);
}
void QueueMain::_timerStopHandler(struct k_timer *timer)
{
    LOG_DBG("timer=%p", timer);
}
