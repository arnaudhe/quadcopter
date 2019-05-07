# Embedded Software

## Overview

The embedded software runs on a ESP32, on a ESP32-WROOM development board.

This boards embeds an ESP32-VROOM module (which contains ESP32 MCU + wifi + BLE + flash), a power supply, a usb-serial interface, and a micro-USB port.

The project is based on Espressif's official [ESP-IDF SDK](https://github.com/espressif/esp-idf), which comes with many components (FreeRTOS, LwIP, utilities, ...). This third-party sdk is actually integrated as a git sumbodule.

## Setup

### SDK

```
# git submodule update --init --recursive
```

### Toolchain

Get the toochain provided by espressif [here](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#step-1-set-up-the-toolchain)

Make sure to have the toolchain in your system path or edit your bashrc, zshrc, ...
```
# xtensa-esp32-elf-gcc -v
(...)
gcc version 5.2.0 (crosstool-NG crosstool-ng-1.22.0-80-g6c4433a)
```

### Python tools

Make sure to have python 2.x and pip in your system path when running command
```
# python --version
Python 2.7.10
```

```
# pip --version
pip 19.1 from /Library/Python/2.7/site-packages/pip (python 2.7)
```

Install necessary packages with the command
```
# sudo pip install -r esp-idf/requirements.txt
```

### CMake - Make

Make sure to have CMake or GNU Make installed and in your path.

## Build and flash

### CMake

```
# export IDF_PATH=`pwd`/esp-idf
# export PATH="$IDF_PATH/tools:$PATH"
# idf.py build
# idf.py flash [-p <port>]
```

### Make (Legacy)

```
# make
# make flash [ESPPORT=<port>]
```