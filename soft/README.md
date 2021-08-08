# Embedded Software

## Overview

The embedded software runs on a ESP32, on a ESP32-WROOM development board.

This boards embeds an ESP32-VROOM module (which contains ESP32 MCU + wifi + BLE + flash), a power supply, a usb-serial interface, and a micro-USB port.

The project is based on Espressif's official [ESP-IDF SDK](https://github.com/espressif/esp-idf), which comes with many components (FreeRTOS, LwIP, utilities, ...). This third-party sdk is actually integrated as a git sumbodule.

## Setup

### Get the SDK

```
# git submodule update --init --recursive
```

### Python tools

Make sure to have python 3 and pip in your system path when running command
```
# python3 --version
Python 3.9.4
```

### Setup the espressif toolchain and tools

Once SDK cloned, install espressif toolchain and tools with

```
./esp-idf/install.sh
```

This will create a folder `.espressif` in your home folder, and:
* download the espressif toolchain, bin utils and debug utils
* setup a python virtual environement we will work into

Note: Alternatively, you can consider setup the environement through vscode IDF extensions. However, SDK as submodule is not managed by the extension.

## Build, flash, monitor

### Setup the environement

```
# source ./env.sh
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

### Extensions

Install ["C/C++" extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-) in Visual Studio Code

### Features

Open the folder `soft` in Visual Studio Code. You may already have:
* C/C++ indexing and completion
* Integrated build

Don't forget to run Visual Studio in espressif python virtual environment

### Build, Flash, Monitor

Hit `cmd + shift + B` to build the project. The errors and warnings should be highlighted in the "problems" tab.
