# Flight Controller

## Overview

The flight controller firmware runs on a ESP32, on a ESP32-WROOM development board.

This boards embeds an ESP32-VROOM module (which contains ESP32 MCU + wifi + BLE + flash), a power supply, a usb-serial interface, and a micro-USB port.

The project is based on Espressif's official [ESP-IDF SDK](https://github.com/espressif/esp-idf), which comes with many components (FreeRTOS, LwIP, utilities, ...). This third-party sdk is actually integrated as a git sumbodule.

## Setup

Flight controller SDK setup is managed by parent folder `software`, refer to its README

## Build, flash, monitor

### Setup the environement

```
# source ../env.sh
```

### Build

```
# idf.py build
```

### Flash

```
# idf.py flash [-p <port>]
```

### Monitor

```
# idf.py monitor [-p <port>]
```
Hit `ctrl + $` to exit

You can alternatively use any serial port util at 921600 bauds

## Visual Studio Code integration

Previous commands are configured as Visual Studio Code tasks.

Refer to parent folder READLE to setup Visual Studio Code and run those tasks

Then, hit `cmd + shift + B` to build the project. The errors and warnings should be highlighted in the "problems" tab.