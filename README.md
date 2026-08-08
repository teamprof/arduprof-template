## ArduProf template

Writing boilerplate code for different MCUs and operating systems is often a tedious, repetitive task that drains developer productivity. Each platform demands its own initialization routines, peripheral drivers, and OS-specific abstractions, forcing engineers to reinvent the wheel before they can even begin solving the real problem. This fragmentation slows down innovation and makes starting a new project unnecessarily complex. A unified framework that abstracts away these differences can dramatically simplify development, allowing engineers to focus on application logic instead of low-level setup. By standardizing common components and offering cross-platform APIs, such a framework empowers developers to spin up new projects quickly, reduces onboarding friction, and accelerates time-to-market.

Leveraging the arduprof-template brings significant benefits to embedded developers by unifying support across multiple platforms. Instead of juggling separate boilerplate setups for Arduino FreeRTOS on ESP32, Arduino Mbed/FreeRTOS on Raspberry Pi Pico/Pico2, and Zephyr on nRF52840, developers can rely on a single, consistent project structure. This reduces friction when switching between MCUs and operating systems, accelerates prototyping, and ensures portability of application logic. By abstracting away repetitive initialization and OS-specific details, the template allows engineers to focus on building features rather than reworking infrastructure. Ultimately, it streamlines cross-platform development, shortens project startup time, and fosters cleaner, more maintainable codebases.


[![License: GPL v3](https://img.shields.io/badge/License-GPL_v3-blue.svg)](https://github.com/teamprof/ArduProf/blob/main/LICENSE)

<a href="https://www.buymeacoffee.com/teamprof" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me A Coffee" style="height: 28px !important;width: 108px !important;" ></a>


---
## supported Hardware (ESP32, ESP32-S3, ESP32C3, ESP32C6, Pi Pico/Pico2)
The following boards are supported by this project:
- [FireBeetle 2 ESP32-C6 IoT Development Board](https://bit.ly/452Ueoq)
- [ESP32-DevKitC V1](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-devkitc.html)
- [ESP32-S3-DevKitC-1 v1.1](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html)
- [ESP32C3-CORE](https://wiki.luatos.com/chips/esp32c3/board.html)
- [XIAO ESP32C3](https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/)
- [Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)
- [W5100S-EVB-Pico](https://www.wiznet.io/product-item/w5100s-evb-pico/)
- [WizFi360-EVB-Pico](https://www.wiznet.io/product-item/wizfi360-evb-pico/)
- [Raspberry Pi Pico2](https://www.raspberrypi.com/products/raspberry-pi-pico-2/)
- [Noridic nRF52840 DK](https://www.nordicsemi.com/Products/Development-hardware/nRF52840-DK)
- [ProMicro nRF52840](https://www.nologo.tech/product/otherboard/NRF52840.html)


---
## Software setup for ESP32, ESP32-S3, ESP32C3
- Install [Arduino IDE 2.3.7+ for Arduino](https://www.arduino.cc/en/Main/Software)
- Install [Arduino-ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html)
- Install [Arduino DebugLog](https://www.arduino.cc/reference/en/libraries/debuglog/)
- Install [ArduinoJson v7+](https://github.com/bblanchon/ArduinoJson)
- Install [ArduProf v2.3.1+](https://www.arduino.cc/reference/en/libraries/arduprof/)

## Software setup for Raspherry Pi Pico/Pico2 
- Install [Arduino IDE 2.3.7+ for Arduino](https://www.arduino.cc/en/Main/Software)
- Install [Arduino Mbed OS RP2040 Boards 4.0.6+](https://github.com/arduino/ArduinoCore-mbed)
- Install [Arduino Pico 5.4.1+](https://github.com/earlephilhower/arduino-pico)
- Install [Arduino DebugLog](https://www.arduino.cc/reference/en/libraries/debuglog/)
- Install [ArduinoJson v7+](https://github.com/bblanchon/ArduinoJson)
- Install [ArduProf v2.3.1+](https://www.arduino.cc/reference/en/libraries/arduprof/)

## Software setup for Nordic nRF52840
- Install [nRF Connect Toolchain and SDK v3.3.0+](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/installation/install_ncs.html)
- Install [ArduProf v2.3.1+](https://www.arduino.cc/reference/en/libraries/arduprof/)
---


## Pi Pico/Pico2 FreeRTOS
### Build firmware
- clone this repo by "git clone https://github.com/teamprof/arduprof-template.gif"
- Launch Arduino IDE
- Open "arduino-app.ino" under "arduino-app" folder
- On Arduino IDE, click menu "Tools" -> "Board: " -> "Board Manager..." -> "Raspberry Pi Pico - Raspberry Pi Pico/RP2040/RP2350" or "Raspberry Pi Pico 2"
- On Arduino IDE, click menu "Tools" -> "Debug Port" -> "Serial"
- On Arduino IDE, click menu "Tools" -> "CPU Speed" -> "133 MHz" or "150 MHz" for Pi Pico2
- On Arduino IDE, click menu "Tools" -> "Operation System" -> "FreeRTOS SMP"
- Build the code by clicking Menu -> “Sketch” -> “Compile/Verify”   
If everything goes smoothly, you should see the following screen.  
#### Pi Pico (FreeRTOS)
[![build-pico](./assets/build-pi-pico-freertos.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-pi-pico-freertos.png)

#### Pi Pico2 (FreeRTOS)
[![build-pico2](./assets/build-pi-pico2.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-pi-pico2.png)

### Upload firmware
- On Arduino IDE, click Menu -> “Sketch” -> “Upload”  
If everything goes smoothly, you should see the following screen.  

#### Pi Pico (FreeRTOS)
[![upload-pico](./assets/upload-pi-pico-freertos.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/upload-pi-pico-freertos.png)

#### Pi Pico2 (FreeRTOS)
[![upload-pico2](./assets/upload-pi-pico2.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/upload-pi-pico2.png)

### Run firmware
- On Arduino IDE, open Serial monitor and then reset the board  
If everything goes smoothly, you should see the following screen.  

#### Pi Pico (FreeRTOS)
[![run-pico](./assets/run-pi-pico-freertos.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/run-pi-pico-freertos.png)

#### Pi Pico2 (FreeRTOS)
[![run-pico2](./assets/run-pi-pico2.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/run-pi-pico2.png)


---
## Pi Pico Mbed OS
- Open "arduino-app.ino" under "arduino-app" folder under Arduino IDE
- On Arduino IDE, click menu "Tools" -> "Board: " -> "Board Manager..." -> "Raspberry Pi Pico - Arduino Mbed OS RP2040 Boards"
- On Arduino IDE, click menu "Tools" -> "Debug Port" -> "Serial"
- On Arduino IDE, click menu "Tools" -> "CPU Speed" -> "133 MHz"
- On Arduino IDE, click menu "Tools" -> "Operation System" -> "Mbed OS"
- Build the code by clicking Menu -> “Sketch” -> “Compile/Verify”   
If everything goes smoothly, you should see the following screen.  
#### Pi Pico (Mbed OS)
[![build-pico](./assets/build-pi-pico-mbed.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-pi-pico-mbed.png)

### Upload firmware
Note that uploading the Pi Pico Mbed firmware through the IDE may be challenging. A more robust approach is to drag the generated .uf2 file directly onto the Pi Pico board.


### Run firmware
- On Arduino IDE, open Serial monitor and then reset the board  
If everything goes smoothly, you should see the following screen.  

#### Pi Pico (Mbed OS)
[![run-pico](./assets/run-pi-pico-mbed.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/run-pi-pico-mbed.png)


---
## ESP32S3/ESP32C3/ESP32 (FreeRTOS)
- Open "arduino-app.ino" under "arduino-app" folder under Arduino IDE
- On Arduino IDE, click menu "Tools" -> "Board: " -> "Board Manager..." ->  
    "RAKwirelss RAK3112" or  
    "ESP32S3 Dev Module" or  
    "XIAO_ESP32C3" or  
    "ESP32C3 Dev Module" or  
    "DOIT ESP32 DEVKIT V1" or  
    "ESP32 Dev Module"
- On Arduino IDE, click menu "Tools" -> "USB CDC On Boot:" -> "Enable"
- Build the code by clicking Menu -> “Sketch” -> “Compile/Verify”   
If everything goes smoothly, you should see the following screen.  
#### ESP32S3 Dev Module (FreeRTOS)
[![build-esp32s3](./assets/build-esp32s3-dev-module.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-esp32s3-dev-module.png)

#### ESP32C3 Dev Module (FreeRTOS)
[![build-esp32c3](./assets/build-esp32c3-dev-module.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-esp32c3-dev-module.png)

#### Xiao ESP32C3 (FreeRTOS)
[![build-xiao-esp32c3](./assets/build-xiao-esp32c3.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-xiao-esp32c3.png)

#### ESP32 Dev Module (FreeRTOS)
[![build-esp32](./assets/build-esp32-dev-module.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-esp32-dev-module.png)

#### DOIT ESP32 DevKit v1 (FreeRTOS)
[![build-esp32-devkit](./assets/build-doit-esp32-devkit-v1.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-doit-esp32-devkit-v1.png)

### Upload firmware
- On Arduino IDE, click Menu -> “Sketch” -> “Upload”  
If everything goes smoothly, you should see the following screen.  

#### ESP32S3 Dev Module (FreeRTOS)
[![upload-esp32s3](./assets/upload-esp32s3-dev-module.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/upload-esp32s3-dev-module.png)

#### Xiao ESP32C3 (FreeRTOS)
[![upload-xiao-esp32c3](./assets/upload-xiao-esp32c3.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/upload-xiao-esp32c3.png)

#### DOIT ESP32 DevKit v1 (FreeRTOS)
[![upload-esp32-devkit](./assets/upload-doit-esp32-devkit-v1.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/upload-doit-esp32-devkit-v1.png)


### Run firmware
- On Arduino IDE, open Serial monitor and then reset the board  
If everything goes smoothly, you should see the following screen.  

#### ESP32S3 Dev Module (FreeRTOS)
[![run-esp32s3](./assets/run-esp32s3-dev-module.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/upload-esp32s3-dev-module.png)

#### Xiao ESP32C3 (FreeRTOS)
[![run-xiao-esp32c3](./assets/run-xiao-esp32c3.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/run-xiao-esp32c3.png)

#### DOIT ESP32 DevKit v1 (FreeRTOS)
[![run-esp32-devkit](./assets/run-doit-esp32-devkit-v1.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/run-doit-esp32-devkit-v1.png)


---
## Nordic nRF52840 (Zephyr)
### Build firmware
- clone this repo by "git clone https://github.com/teamprof/arduprof-template.gif"
- change to "nrf" folder and download submodule
```
    cd nrf
    git submodule update --init --recursive
```
- Launch VS code, switch to nRF Connect panel, open existing application under "nrf" folder

#### Noridic nRF52840 DK
- Add or edit build config, as below  
[![build-dk-config](./assets/build-nrf52840-dk-config.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-nrf52840-dk-config.png)

- Click "Generate and Build"
If everything goes smoothly, you should see the following screen.  
[![build-dk](./assets/build-nrf52840-dk.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-nrf52840-dk.png)

#### ProMicro nRF52840
- Add or edit build config, as below  
[![build-promicro52840-config](./assets/build-promicro52840-config.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-promicro52840-config.png)

- Click "Generate and Build"
If everything goes smoothly, you should see the following screen.  
[![build-promicro52840](./assets/build-promicro52840.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-promicro52840.png)



### Run firmware
#### Noridic nRF52840 DK

- Click "Debug" on nRF Connect panel
If everything goes smoothly, you should see the following screen    
[![run-dk](./assets/run-nrf52840-dk.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/run-nrf52840-dk.png)

- Launch a serial terminal likes TeraTerm or minicom, set as 8N1 115200bps  
If everything goes smoothly, you should see the following screen if you use TeraTerm.  
[![teraterm-dk](./assets/teraterm-nrf52840-dk.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/teraterm-nrf52840-dk.png)


#### ProMicro nRF52840
- Double click ProMicro nRF52840 Reset pin within 0.5s to enter Bootloder mode
- Copy the "zephyr.uf2" under ""folder to the drive of the board
- Launch a Serial Terminal software likes TeraTerm, set to 115200bps and 8N1  
If everything goes smoothly, you should see the following screen  if you use TeraTerm.  
[![teraterm-promicro52840](./assets/teraterm-promicro52840.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/teraterm-promicro52840.png)


---
## Arduino code explanation
In a typical Arduino setup(), both QueueMain and ThreadApp are initialized. QueueMain runs within the Arduino loop(), while ThreadApp executes in parallel. The purpose of QueueMain is to establish a communication channel between the Arduino loop() and other threads.
```
static void startTasks(void)
{
    auto ctx = getAppContext();
    if (ctx->queueMain)
    {
        static_cast<QueueMain *>(ctx->queueMain)->start(ctx);
    }
    if (ctx->threadApp)
    {
        ctx->threadApp->start(ctx);
    }
}

void loop()
{
    ...

    auto ctx = getAppContext();
    auto qMain = static_cast<QueueMain *>(ctx->queueMain);
    assert(qMain);
    qMain->messageLoop(0); // non-blocking
    // qMain->messageLoop();  // blocking until event received and proceed
    // qMain->messageLoopForever(); // never return

    // delay(1000);    // delay 1s
    delay(100); // delay 0.1s
}
```

## Add new thread(s)
- Copy the files "ThreadApp.cpp" and "ThreadApp.h" and rename them, for example "ThreadGui"
- Edit "ThreadGui.h", modify the CLASSNAME macro as below: 
``` 
#define CLASSNAME ThreadGui
```
- Edit "AppContext.h", modify the AppContext struct as below: 
```
    typedef struct _AppContext
    {
        ardufreertos::MessageQueue *queueMain;
        ardufreertos::ThreadBase *threadApp;
        ardufreertos::ThreadBase *threadGui;
    } AppContext;

    typedef struct _AppContext
    {
        ardumbedos::MessageQueue *queueMain;
        ardumbedos::ThreadBase *threadApp;
        ardumbedos::ThreadBase *threadGui;  // <= add code here
    } AppContext;
```
- Edit "AppContext.cpp", modify the AppContext code as below: 
```
    static AppContext appContext = {
        .queueMain = &queueMain,
        .threadApp = ThreadApp::getInstance(),
        .threadGui = ThreadGui::getInstance(),  // <= add code here
    };
```
- Edit "arduino-app.ino", modify the startTasks code as below: 
```
static void startTasks(void)
{
    auto ctx = getAppContext();
    if (ctx->queueMain)
    {
        static_cast<QueueMain *>(ctx->queueMain)->start(ctx);
    }
    if (ctx->threadApp)
    {
        ctx->threadApp->start(ctx);
    }

    //////////////////////////////////////////////////////
    // add code here
    //////////////////////////////////////////////////////
    if (ctx->threadGui)
    {
        ctx->threadGui->start(ctx);
    }
}
```
- The new ThreadGui is added. Build the updated code, upload the new firmware, the new thread should run in parellel with ThreadApp and QueueMain.
- Please refer to [ArduProf](https://github.com/teamprof/arduprof) about sending and handling messages between threads

---
### License
- The project is licensed under GNU GENERAL PUBLIC LICENSE Version 3
---

### Copyright
- Copyright 2026 teamprof.net@gmail.com. All rights reserved.
---



