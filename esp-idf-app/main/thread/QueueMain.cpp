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

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
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
void CLASSNAME::printChipInfo(void)
{
#ifdef ARDUINO    
    PRINTLN("===============================================================================");
    PRINTLN("App Firmware version=", AppVersion::getFirmwareVersionString());
#if defined ARDUINO_ARCH_RP2040
    PRINTLN("rp2040_chip_version():", rp2040_chip_version(), ", rp2040_rom_version():", rp2040_rom_version(),
            "\r\nPSRAM total size:", rp2040.getPSRAMSize(),
            "\r\ntotal heap:", rp2040.getTotalHeap(), ", free heap:", rp2040.getFreeHeap());
#elif defined ARDUINO_ARCH_ESP32
    PRINTLN("ESP.getChipModel()=", ESP.getChipModel(), ", getChipRevision()=", ESP.getChipRevision(), ", getFlashChipSize()=", ESP.getFlashChipSize(),
            "\r\nNumber of cores=", ESP.getChipCores(), ", SDK version=", ESP.getSdkVersion(),
            "\r\nPSRAM total size=", ESP.getPsramSize(), " bytes, PSRAM free size=", ESP.getFreePsram(), " bytes");
#endif
    PRINTLN("===============================================================================");

#elif defined ESP_PLATFORM
    // Initialize the chip info structure
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    // Get SDK Version
    // This returns a string like "v5.x.x"
    const char *idf_version = esp_get_idf_version();

    // // Get Flash Size
    // // esp_flash_default_chip gets the main flash chip configured in your project
    // uint32_t flash_size = 0;
    // if (esp_flash_get_size(esp_flash_default_chip, &flash_size) != ESP_OK) {
    //     ESP_LOGE(TAG, "Failed to get flash size");
    // }

    // Extract major and minor revision from the revision field (format MXX: M=major, XX=minor)
    uint16_t rev = chip_info.revision;
    uint32_t major_rev = rev / 100;
    uint32_t minor_rev = rev % 100;

    ESP_LOGI(TAG, "===============================================================================");
    ESP_LOGI(TAG, "App Firmware version=%s", AppVersion::getFirmwareVersionString());
    ESP_LOGI(TAG, "Chip Model ID=%d, Chip Version/Revision: v%d.%d (Raw value: %u)"
        "\r\n\tNumber of cores=%d, SDK version=%s",
        // "\r\nFlash Size: %lu MB (%lu bytes)", 
        chip_info.model, (int)major_rev, (int)minor_rev, rev, 
        chip_info.cores, idf_version
        // flash_size / (1024 * 1024), flash_size
    );
        //     "\r\nPSRAM total size=", ESP.getPsramSize(), " bytes, PSRAM free size=", ESP.getFreePsram(), " bytes");
    ESP_LOGI(TAG, "===============================================================================");
    // (ID) (Enum Value)    
    // 1    CHIP_ESP32      ESP32
    // 2    CHIP_ESP32S2    ESP32-S2
    // 5    CHIP_ESP32C3    ESP32-C3 (RISC-V)
    // 9    CHIP_ESP32S3    ESP32-S3 
    // 12   CHIP_ESP32C2    ESP32-C2
    // 13   CHIP_ESP32C6    ESP32-C6
    // 16   CHIP_ESP32H2    ESP32-H2

  
        // !!!
//     // 4. Get PSRAM Size
//     size_t psram_size = 0;
// #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
//     if (esp_psram_is_initialized()) {
//         psram_size = esp_psram_get_size();
//     }
// #else
//     // For ESP-IDF v4.x
//     if (esp_spiram_is_initialized()) {
//         psram_size = esp_spiram_get_size();
//     }
// #endif

    // if (psram_size > 0) {
    //     ESP_LOGI(TAG, "PSRAM Size:       %d MB (%d bytes)", psram_size / (1024 * 1024), psram_size);
    // } else {
    //     ESP_LOGI(TAG, "PSRAM Size:       No PSRAM detected or enabled");
    // }


    // // Print basic hardware information
    // ESP_LOGI(TAG, "This is ESP32-S3 chip with %d CPU core(s)", chip_info.cores);

    // ESP_LOGI(TAG, "Chip Model ID: %d (CHIP_ESP32S3 is usually 9)", chip_info.model);
    // ESP_LOGI(TAG, "Chip Version/Revision: v%d.%d (Raw value: %u)", (int)major_rev, (int)minor_rev, rev);

    // Check features mask
    if (chip_info.features & CHIP_FEATURE_EMB_PSRAM) {
        ESP_LOGI(TAG, "Embedded PSRAM is present");
    }
    if (chip_info.features & CHIP_FEATURE_EMB_FLASH) {
        ESP_LOGI(TAG, "Embedded Flash is present");
    }

#endif
}

#elif defined ARDUPROF_MBED
////////////////////////////////////////////////////////////////////////////////////////////
// Thread for Pi Pico (RP2040) Mbed OS
////////////////////////////////////////////////////////////////////////////////////////////
#define THREAD_QUEUE_SIZE (128 * EVENTS_EVENT_SIZE) // message queue size for app thread

void CLASSNAME::printChipInfo(void)
{
    PRINTLN("===============================================================================");
    PRINTLN("App Firmware version=", AppVersion::getFirmwareVersionString());
    PRINTLN("rp2040_chip_version()=", rp2040_chip_version(), ", rp2040_rom_version()=", rp2040_rom_version());
    PRINTLN("===============================================================================");
}

/////////////////////////////////////////////////////////////////////////////
// use static threadQueue instead of heap
static events::EventQueue threadQueue(THREAD_QUEUE_SIZE);

#endif

/////////////////////////////////////////////////////////////////////////////
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
#if defined ARDUPROF_FREERTOS 
#if defined ARDUINO_ARCH_RP2040
    LOG_TRACE("core", get_core_num(), ", uxTaskPriorityGet(NULL)=", uxTaskPriorityGet(NULL));
#elif defined ARDUINO_ARCH_ESP32
    LOG_TRACE("on core ", xPortGetCoreID(), ", xPortGetFreeHeapSize()=", xPortGetFreeHeapSize());
#elif defined ESP_PLATFORM
    ESP_LOGV(TAG, "on core %d, xPortGetFreeHeapSize()=%u", xPortGetCoreID(), xPortGetFreeHeapSize());
#endif
#elif defined ARDUPROF_MBED
    LOG_TRACE("Mbed OS thread started");
#endif

    MessageBus::start(ctx);

    printChipInfo();
    // LOG_DEBUG("uxTaskPriorityGet(NULL)=", uxTaskPriorityGet(NULL));

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
        ESP_LOGW(TAG, "Unsupported event=%d, iParam=%d, uParam=%d, lParam=%lu", msg.event, msg.iParam, msg.uParam, msg.lParam);
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
        ESP_LOGW(TAG, "unsupported SystemTriggerSource=%d", src);
        break;
    }
}

// define EventNull handler
__EVENT_FUNC_DEFINITION(CLASSNAME, EventNull, msg) // void CLASSNAME::handlerEventNull(const Message &msg)
{
    ESP_LOGD(TAG, "EventNull(%d), iParam=%d, uParam=%u, lParam=%lu", msg.event, msg.iParam, msg.uParam, msg.lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CLASSNAME::handlerSoftwareTimer(TimerHandle_t xTimer)
{
    if (xTimer == _timer1Hz.timer())
    {
        ESP_LOGV(TAG, "_timer1Hz");
    }
    else
    {
        ESP_LOGW(TAG, "unsupported timer handle=%p", xTimer);
    }
}
