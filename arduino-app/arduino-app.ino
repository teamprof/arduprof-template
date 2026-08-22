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
/*
For ESP32/ESP32C3/ESP32S3, please use the following settings in Arduino IDE:
- On Arduino IDE, click menu "Tools" -> "Board: " -> "Board Manager..." -> 
    "DFRobot FireBeetle 2 ESP32-C6" or
    "RAKwirelss RAK3112" or
    "ESP32S3 Dev Module" or
    "XIAO_ESP32C3" or
    "ESP32C3 Dev Module" or
    "DOIT ESP32 DEVKIT V1" or
    "ESP32 Dev Module"
- On Arduino IDE, click menu "Tools" -> "USB CDC On Boot:" -> "Enable"

For Pi Pico (RP2040) Mbed, please use the following settings in Arduino IDE:
- On Arduino IDE, click menu "Tools" -> "Board: " -> "Board Manager..." -> "Raspberry Pi Pico - Arduino Mbed OS RP2040 Boards"
- On Arduino IDE, click menu "Tools" -> "Debug Port" -> "Serial"
- On Arduino IDE, click menu "Tools" -> "CPU Speed" -> "133 MHz"
- On Arduino IDE, click menu "Tools" -> "Operation System" -> "Mbed OS"

For Pi Pico2 (RP2350) with FreeRTOS SMP, please use the following settings in Arduino IDE:
- On Arduino IDE, click menu "Tools" -> "Board: " -> "Board Manager..." -> "Raspberry Pi Pico - Raspberry Pi Pico/RP2040/RP2350" or "Raspberry Pi Pico 2"
- On Arduino IDE, click menu "Tools" -> "Debug Port" -> "Serial"
- On Arduino IDE, click menu "Tools" -> "CPU Speed" -> "133 MHz" or "150 MHz" for Pi Pico2
- On Arduino IDE, click menu "Tools" -> "Operation System" -> "FreeRTOS SMP"
*/
#include <Arduino.h>

#include "./src/ArduProfApp.h"
#include "./src/AppContext.h"
#include "./src/thread/QueueMain.h"

///////////////////////////////////////////////////////////////////////////////
static void initGlobalVar(void)
{
}

static void startTasks(void)
{
    auto &ctx = getAppContext();
    if (ctx.queueMain)
    {
        static_cast<QueueMain *>(ctx.queueMain)->start(&ctx);
    } else {
        LOG_DEBUG("ctx.queueMain is NULL");
    }

    if (ctx.threadApp)
    {
        ctx.threadApp->start(&ctx);
    } else {
        LOG_DEBUG("ctx.threadApp is NULL");
    }

    if (ctx.threadButton) 
    {
        ctx.threadButton->start(&ctx);
    } 
    else 
    {
        LOG_DEBUG("ctx.threadButton is NULL");
    }
}

// set debug port to USB/CDC if USB/CDC is found without INIT_DEBUG_PORT_TIMEOUT
// otherwise, set debug port to Serial0 (UART0)
#define INIT_DEBUG_PORT_TIMEOUT 3000 // in unit of ms
static void initDebugPort(void)
{
    int timeout = 0;

    // Serial.setDebugOutput(false);
    Serial.begin(115200);
    while (!Serial && timeout < INIT_DEBUG_PORT_TIMEOUT)
    {
        delay(100);
        timeout += 100;
    }
    delay(500);

    /////////////////////////////////////////////////////////////////////////////
    // set log output to serial port, and init log params such as log_level
    LOG_SET_LEVEL(DefaultLogLevel);
    // LOG_SET_LEVEL(DebugLogLevel::LVL_TRACE); // enable debug log
    // LOG_SET_LEVEL(DebugLogLevel::LVL_NONE);  // disable debug log
    LOG_SET_DELIMITER("");
    /////////////////////////////////////////////////////////////////////////////

    if (Serial)
    {
        LOG_ATTACH_SERIAL(Serial);
        LOG_TRACE("set debug port to USB/CDC");
    }
    else
    {
        Serial1.begin(115200);
        LOG_ATTACH_SERIAL(Serial1);
        LOG_TRACE("set debug port to UART0");
    }
}

///////////////////////////////////////////////////////////////////////////////
void setup()
{
    initDebugPort();

    initGlobalVar();
    startTasks();
}

void loop()
{
    // static int count = 0;
    // LOG_TRACE("count=", count++);

    auto& ctx = getAppContext();
    auto qMain = static_cast<QueueMain *>(ctx.queueMain);
    assert(qMain);
    qMain->messageLoop(0); // non-blocking
    // qMain->messageLoop();  // blocking until event received and proceed
    // qMain->messageLoopForever(); // never return

    // delay(1000);    // delay 1s
    delay(100); // delay 0.1s
}
