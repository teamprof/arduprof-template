
// # UART0 as debug log port
// CONFIG_ESP_CONSOLE_UART_DEFAULT=y

// # USB CDC as debug log port
// CONFIG_ESP_CONSOLE_UART_DEFAULT=n
// # CONFIG_ESP_CONSOLE_USB_CDC=y
// CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y


// #include <stdio.h>
#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
// #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
static const char *TAG = "main";

#include "./ArduProfApp.h"
#include "./AppContext.h"
#include "./thread/QueueMain.h"
#include "./peripheral/unused_pins.h"


#ifdef __cplusplus
extern "C" void app_main(void);
#endif

static void initGlobalVar(void)
{
}

static void startTasks(void)
{
    ESP_LOGV(TAG, "startTasks...");

    auto& ctx = getAppContext();
    if (ctx.queueMain) {
        static_cast<QueueMain *>(ctx.queueMain)->start(&ctx);
    } else {
        ESP_LOGW(TAG, "ctx->queueMain is NULL");
    }
    if (ctx.threadApp) {
        ctx.threadApp->start(&ctx);
    } else {
        ESP_LOGW(TAG, "ctx.threadApp is NULL");
    }
    if (ctx.threadButton) {
        ctx.threadButton->start(&ctx);
    } else {
        ESP_LOGW(TAG, "ctx.threadButton is NULL");
    }
}

static void setup(void)
{
    esp_log_level_set("*", ESP_LOG_VERBOSE);

#if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT)    
    ESP_LOGI(TAG, "debug port is Hardware UART0 (Default)");
#elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOGI(TAG, "debug port is Internal USB Serial/JTAG Controller");
    vTaskDelay(pdMS_TO_TICKS(1000));
#elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOGI(TAG, "debug port is USB CDC (Virtual COM via TinyUSB)");
#endif

    unused_pins::setup();

    initGlobalVar();
    startTasks();
}

static void loop(void)
{
    auto& ctx = getAppContext();
    auto qMain = static_cast<QueueMain *>(ctx.queueMain);
    if(qMain)
    {
        // qMain->messageLoop(0); // non-blocking
        // qMain->messageLoop();  // blocking until event received and proceed
        qMain->messageLoopForever(); // never return
    }
    vTaskDelay(pdMS_TO_TICKS(100));

    static uint32_t count = 0;
    ESP_LOGI(TAG, "Count: %lu", count++);
    vTaskDelay(pdMS_TO_TICKS(1000));
}

///////////////////////////////////////////////////////////////////////////////
void app_main(void)
{
    setup();
    while (true) {
        loop();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}