## Extending Pi Pico/Pico 2 Dual-Core Features with W5500 Ethernet Connectivity on NuttX OS

This repository demonstrates how to extend Ethernet connectivity on the Pi Pico and Pico 2 using the WIZnet W5500 silicon. In addition, it compares the iperf results of the Pi Pico and Pi Pico 2 running on single or dual cores.

**System diagram**
```
 ┌─────────────┐              ┌──────────────┐ 
 │             │◄─── wifi ───►│     PC       │ 
 │             │              │ iperf client │ 
 │             │              └──────────────┘ 
 │   Router    │                               
 │             │              ┌──────────────┐ 
 │             │◄─── eth  ───►│ Pico/Pico2   │ 
 │             │              │ iperf server │ 
 └─────────────┘              └──────────────┘ 
```

note: The iperf client results on the Pico and Pico 2 drop significantly due to the missing '-l' feature in NuttX's iperf implementation.

---

[![License: GPL v3](https://img.shields.io/badge/License-GPL_v3-blue.svg)](https://github.com/teamprof/ArduProf/blob/main/LICENSE)

<a href="https://www.buymeacoffee.com/teamprof" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me A Coffee" style="height: 28px !important;width: 108px !important;" ></a>

---

## Supported Hardware (Pi Pico/Pico2)

The following boards are supported by this project:
- [Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html) + [WIZnet W5500](https://wiznet.io/products/ethernet-chips/w5500)
- [Raspberry Pi Pico2](https://www.raspberrypi.com/products/raspberry-pi-pico-2/) + [WIZnet W5500](https://wiznet.io/products/ethernet-chips/w5500)
- [WIZnet W5500-EVB-Pico](https://wiznet.io/products/evaluation-boards/w5500-evb-pico)*
- [WIZnet W5500-EVB-Pico2](https://wiznet.io/products/evaluation-boards/w5500-evb-pico2)*

note *: Not tested, but it should work because the W5500 connections are identical on both the Pico and Pico 2.

---


## Connections between Raspherry Pi Pico/Pico2 and Wiznet W5500
| WIZnet W5500 | Pi Pico/Pico2 | remarks |
|--------------|---------------|---------|
| VCC  | VCC (3.3V) | VCC |
| GND  | GND        | GND |
| RSTn | GPIO20 | Reset     |
| INTn | GPIO21 | Interrupt |
| CSn  | GPIO17 | SPI CS    |
| SCLK | GPIO18 | SPI SCK   |
| MISO | GPIO16 |  SPI MISO |
| MOSI | GPIO19 |  SPI MOSI |

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
  ./tools/configure.sh -l ../src/boards/arm/rp2040/w5500-evb-pico/configs/nsh      # for Pi Pico RP2040 single core
  # ./tools/configure.sh -l ../src/boards/arm/rp2040/w5500-evb-pico/configs/smp    # for Pi Pico RP2040 dual core
  # ./tools/configure.sh -l ../src/boards/arm/rp23xx/w5500-evb-pico2/configs/nsh   # for Pi Pico RP2350 single core
  # ./tools/configure.sh -l ../src/boards/arm/rp23xx/w5500-evb-pico2/configs/smp   # for Pi Pico RP2350 dual core
  ```
- If everything goes smoothly, you should see as belows:    
  **For Pico RP2040 single core (nsh)**  
  [![config-pico-w5500-nsh](./assets/config-pico-w5500-nsh.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/config-pico-w5500-nsh.png)  
  
  **For Pico RP2040 dual core (smp)**  
  [![config-pico-w5500-smp](./assets/config-pico-w5500-smp.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/config-pico-w5500-smp.png)  

  **For Pico2 RP2350 single core (nsh)**  
  [![config-pico2-w5500-nsh](./assets/config-pico2-w5500-nsh.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/config-pico2-w5500-nsh.png)  

  **For Pico2 RP2350 dual core (smp)**  
  [![config-pico2-w5500-smp](./assets/config-pico2-w5500-smp.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/config-pico2-w5500-smp.png)  

- Build firmware by typing command and press ENTER.
  ```
  make
  ```

- If everything goes smoothly, you should see the following screen:  
  **For Pico RP2040 single core (nsh)**  
  [![build-pico-w5500-nsh](./assets/build-pico-w5500-nsh.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-pico-w5500-nsh.png)

  **For Pico RP2040 dual core (smp)**  
  [![build-pico-w5500-smp](./assets/build-pico-w5500-smp.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-pico-w5500-smp.png)

  **For Pico2 RP2350 single core (nsh)**  
  [![build-pico2-w5500-nsh](./assets/build-pico2-w5500-nsh.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-pico2-w5500-nsh.png)

  **For Pico2 RP2350 dual core (smp)**  
  [![build-pico2-w5500-smp](./assets/build-pico2-w5500-smp.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/build-pico2-w5500-smp.png)


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
- Launch a Serial Terminal app (e.g. minicom) and connect to /dev/ttyACM0 with 8N1, 115200 bps, by the following commands
  ```
  minicom -b 115200 -D /dev/ttyACM0
  ```
- Plug the USB cable back in.
- The Pico/Pico2 will power up and immediately start running the firmware.
- Wait 1 to 2 seconds and then press ENTER in the Serial Terminal app
- Type the following command in the Serial Termianl app to start iperf server on Pico/Pico2  
  ```
  iperf -s
  ```

- Launch another terminal and type the following commands to start iperf client on PC
  ```
  iperf -c 192.168.0.147 -t 10 -l 1460
  ```
- If everything goes smoothly, you should see the following PC screen:  
  **PC screen of Pico RP2040 single core (nsh)**  
  [![run-pc-pico-nsh](./assets/minicom-pc-pico-nsh-iperf-c.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/minicom-pc-pico-nsh-iperf-c.png)

  **PC screen of Pico RP2040 dual core (smp)**  
  [![run-pc-pico-smp](./assets/minicom-pc-pico-smp-iperf-c.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/minicom-pc-pico-smp-iperf-c.png)

  **PC screen of Pico2 RP2350 single core (nsh)**  
  [![run-pc-pico2-nsh](./assets/minicom-pc-pico2-nsh-iperf-c.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/minicom-pc-pico2-nsh-iperf-c.png)

  **PC screen of Pico2 RP2350 dual core (smp)**  
  [![run-pc-pico2-smp](./assets/minicom-pc-pico2-smp-iperf-c.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/minicom-pc-pico2-smp-iperf-c.png)


- the nutshell screen running on Pico/Pico2 should be as follows:  
  **On Pico RP2040 single core (nsh)**  
  [![run-pico-nsh](./assets/minicom-pico-nuttx-nsh-iperf-s.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/minicom-pico-nuttx-nsh-iperf-s.png)

  **On Pico RP2040 dual core (smp)**  
  [![run-pico-smp](./assets/minicom-pico-nuttx-smp-iperf-s.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/minicom-pico-nuttx-smp-iperf-s.png)

  **On Pico2 RP2350 single core (nsh)**  
  [![run-pico2-nsh](./assets/minicom-pico2-nuttx-nsh-iperf-s.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/minicom-pico2-nuttx-nsh-iperf-s.png)

  **On Pico2 RP2350 dual core (smp)**  
  [![run-pico2-smp](./assets/minicom-pico2-nuttx-smp-iperf-s.png)](https://github.com/teamprof/arduprof-template/blob/main/assets/minicom-pico2-nuttx-smp-iperf-s.png)


- Good luck and enjoy NuttX on Pico/Pico2 with W5500

---

### License
- The project is licensed under GNU GENERAL PUBLIC LICENSE Version 3

---

### Copyright
- Copyright 2026 teamprof.net@gmail.com. All rights reserved.
