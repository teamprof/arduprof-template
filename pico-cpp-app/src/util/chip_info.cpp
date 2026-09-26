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

#include "chip_info.h"
#include "../ArduProfApp.h"
#include "../AppVersion.h"


#if defined ARDUINO 
// #if defined ARDUINO_ARCH_ESP32
// #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
// #include "esp_log.h"

#elif defined ARDUPROF_PICO_C
#define LOG_LOCAL_LEVEL APP_LOG_VERBOSE
#include "../AppLog.h"
static const char *TAG = STR(chip_info);

#elif defined ESP_PLATFORM
#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "../AppLog.h"
static const char *TAG = STR(chip_info);

#endif


namespace chip {

    void printInfo(void)
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

#elif defined ARDUPROF_PICO_C
        #if PICO_RP2350
        uint8_t chip_rev = rp2350_chip_version();   // RP2350 chip version (e.g.: 2 means A2, 3 means A3/A4)
        uint8_t rom_rev = rp2350_rom_version();
        #elif PICO_RP2040
        uint8_t chip_rev = rp2040_chip_version();   // RP2040 version (e.g.: 1 means B0/B1, 2 means B2)
        uint8_t rom_rev = rp2040_rom_version();     // RP2040 Boot ROM version (e.g.: 1 means v1, 2 means v2, 3 means v3/B2)
        #endif

        APP_LOGI(TAG, "===============================================================================");
        APP_LOGI(TAG, "App Firmware version=%s", AppVersion::getFirmwareVersionString());
        APP_LOGI(TAG, "Chip Revision: A%d, Boot ROM Version: v%d", chip_rev, rom_rev);
        APP_LOGI(TAG, "===============================================================================");

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

        APP_LOGI(TAG, "===============================================================================");
        APP_LOGI(TAG, "App Firmware version=%s", AppVersion::getFirmwareVersionString());
        APP_LOGI(TAG, "Chip Model ID=%d, Chip Version/Revision: v%d.%d (Raw value: %u)"
            "\r\n\t\t   Number of cores=%d, SDK version=%s",
            // "\r\nFlash Size: %lu MB (%lu bytes)", 
            chip_info.model, (int)major_rev, (int)minor_rev, rev, 
            chip_info.cores, idf_version
            // flash_size / (1024 * 1024), flash_size
        );
            //     "\r\nPSRAM total size=", ESP.getPsramSize(), " bytes, PSRAM free size=", ESP.getFreePsram(), " bytes");
        APP_LOGI(TAG, "===============================================================================");
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

#elif defined ARDUPROF_MBED
    PRINTLN("===============================================================================");
    PRINTLN("App Firmware version=", AppVersion::getFirmwareVersionString());
    PRINTLN("rp2040_chip_version()=", rp2040_chip_version(), ", rp2040_rom_version()=", rp2040_rom_version());
    PRINTLN("===============================================================================");

#endif
    
    }
};

////////////////////////////////////////////////////////////////////////////////////////////
