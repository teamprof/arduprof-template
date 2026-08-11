## Reference code of running NuttX on dual cores of Pi Pico/Pico2 with ArduProf framework

Running Apache NuttX on a Raspberry Pi Pico/Pico2 transforms the modest microcontroller board into a POSIX-compliant, multi-threaded system. It unlocks the full dual-core potential via symmetric multiprocessing (SMP), provides a familiar Unix-like shell environment (NSH), and introduces robust driver abstractions for hardware like I2C, SPI, and ADC.

This repository leverages the ArduProf library to demonstrate a highly efficient, message-driven, multi-threaded application structure. By integrating this library, the system can cleanly segregate tasks into independent threads that communicate via asynchronous messages. This design minimizes thread blocking, optimizes CPU utilization across the Pico’s dual cores, and simplifies complex real-time firmware architectures into manageable, event-driven components.

[![framework diagram](./assets/framework-diagram.png)](https://github.com/teamprof/ArduProf/blob/main/doc/image/framework-diagram.png)

---

[![License: GPL v3](https://img.shields.io/badge/License-GPL_v3-blue.svg)](https://github.com/teamprof/ArduProf/blob/main/LICENSE)

<a href="https://www.buymeacoffee.com/teamprof" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me A Coffee" style="height: 28px !important;width: 108px !important;" ></a>

---

## Supported Hardware (Pi Pico/Pico2)

The following boards are supported by this project:
- [Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)
- [Raspberry Pi Pico2](https://www.raspberrypi.com/products/raspberry-pi-pico-2/)

---


## Reference code setup for Raspherry Pi Pico/Pico2
- Launch a terminal app
- clone this repo by "git clone --recurse-submodules https://github.com/teamprof/arduprof-template.gif"
- Change to folder "arduprof-template/pico-nuttx-app" and create a symbolic link
  ```
  cd arduprof-template/pico-nuttx-app
  ln -s ../src apps/arduprof
  ```
---

## Build reference code for Pi Pico/Pico2
- Change to "arduprof-template/pico-nuttx-app/nuttx" folder in terminal.
  ```
  cd nuttx
  ```
- Type the following command and press ENTER.
  ```
  ./tools/configure.sh -l ../src/boards/arm/rp2040/raspberrypi-pico/configs/smp      # for Pi Pico RP2040
  # ./tools/configure.sh -l ../src/boards/arm/rp23xx/raspberrypi-pico-2/configs/smp  # for Pi Pico2 RP2350
  ```
- If everything goes smoothly, you should see the following screen:  
  On Pico RP2040
  [![config-pico-smp](./assets/config-pico-smp.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/config-pico-smp.png)  

  On Pico2 RP2350
  [![config-pico2-smp](./assets/config-pico2-smp.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/config-pico2-smp.png)  

- Build firmware by typing command and press ENTER.
  ```
  make
  ```

- If everything goes smoothly, you should see the following screen:  
  On Pico RP2040 
  [![build-pico-smp](./assets/build-pico-smp.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-pico-smp.png)

  On Pico2 RP2350 
  [![build-pico2-smp](./assets/build-pico2-smp.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-pico2-smp.png)


## Flash reference code on Pi Pico/Pico2
- Press and hold the BOOTSEL button on the Pi Pico/Pico2.
- Plug the USB cable into the Pico/Pico2 and your PC.
- Release the BOOTSEL button after it is plugged in.
- Launch a terminal and change to the "pico-nuttx-app/nuttx" folder.
- Type the following command and press ENTER.
  ```
  cp nuttx.uf2 /media/teamprof/RPI-RP2    # for Pi Pico RP2040
  # cp nuttx.uf2 /media/teamprof/RP2350   # for Pi Pico2 RP2350
  ```

## Run reference code on Pi Pico/Pico2
- Unplug the USB cable from the Pico/Pico2 or your computer.
- Connect your Pico/Pico2's UART0 TX (GP0) and RX (GP1) pins to the RX and TX pins of an USB-to-TTL serial adapter.  
- Launch a Serial Terminal app (e.g. minicom) and connect to /dev/ttyUSB0 with 8N1, 115200 bps, by the following commands
  ```
  minicom -b 115200 -D /dev/ttyUSB0
  ```
- Plug the USB cable back in.
- The Pico/Pico2 will power up and immediately start running the firmware.
- Wait 1 to 2 seconds and then press ENTER in the Serial Terminal app
- If everything goes smoothly, you should see the following screen:  
  [![sh-pico-smp](./assets/sh-pico-smp.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/sh-pico-smp.png)

- Type the following command in the Serial Termianl app
  ```
  arduprof
  ```
- If everything goes smoothly, the on-board will toggle every second and you should see the following screen:  
  On Pico RP2040 
  [![run-pico-smp](./assets/run-pico-smp.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/run-pico-smp.png)

  On Pico2 RP2350 
  [![run-pico2-smp](./assets/run-pico2-smp.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/run-pico2-smp.png)


- Good luck and enjoy NuttX on Pico/Pico2

---

### License
- The project is licensed under GNU GENERAL PUBLIC LICENSE Version 3

---

### Copyright
- Copyright 2026 teamprof.net@gmail.com. All rights reserved.
