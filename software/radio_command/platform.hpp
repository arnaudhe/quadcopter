#pragma once

#define PLATFORM_BUTTON_TOP_RIGHT               23
#define PLATFORM_BUTTON_BOTTOM_RIGHT            24
#define PLATFORM_BUTTON_BOTTOM_LEFT             26
#define PLATFORM_BUTTON_TOP_LEFT                27

#define PLATFORM_YAW_STICK_ADC_CHANNEL          0
#define PLATFORM_THROTTLE_STICK_ADC_CHANNEL     1
#define PLATFORM_ROLL_STICK_ADC_CHANNEL         2
#define PLATFORM_PITCH_STICK_ADC_CHANNEL        3

#define PLATFORM_BATTERY_I2C                    "/dev/i2c-1"
#define PLATFORM_BATTERY_I2C_ADDRESS            0x49
#define PLATFORM_BATTERY_CHANNEL                0

#define PLATFORM_SI4432_SPI                     "/dev/spidev0.0"
#define PLATFORM_SI4432_SPI_SPEED               1000000
#define PLATFORM_SI4432_IRQ                     22
