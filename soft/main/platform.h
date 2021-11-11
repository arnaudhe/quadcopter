#pragma once

#define I2C_MASTER_NUM                        (I2C_NUM_1)         /*!< I2C port number for master dev */

#define PLATFORM_REAR_LEFT_PULSE_CHANNEL      (0)
#define PLATFORM_REAR_LEFT_PULSE_PIN          (2)

#define PLATFORM_FRONT_LEFT_PULSE_CHANNEL     (1)
#define PLATFORM_FRONT_LEFT_PULSE_PIN         (21)

#define PLATFORM_FRONT_RIGHT_PULSE_CHANNEL    (2)
#define PLATFORM_FRONT_RIGHT_PULSE_PIN        (0)

#define PLATFORM_REAR_RIGHT_PULSE_CHANNEL     (3)
#define PLATFORM_REAR_RIGHT_PULSE_PIN         (33)

#define RATE_CONTROLLER_PERIOD                (0.002)
#define ATTITUDE_CONTROLLER_PERIOD            (0.01)
#define HEIGHT_CONTROLLER_PERIOD              (0.025)
#define POSITION_CONTROLLER_PERIOD            (0.1)
#define BATTERY_SUPERVISOR_PERIOD             (1.0)
#define CAMERA_SUPERVISOR_PERIOD              (0.05)

#define PLATFORM_HCSR04_ECHO_PIN              (5)
#define PLATFORM_HCSR04_TRIG_CHANNEL          (4)
#define PLATFORM_HCSR04_TRIG_PIN              (4)

#define PLATFORM_GPS_UART                     (UART_NUM_1)
#define PLATFORM_GPS_RX_PIN                   (GPIO_NUM_22)
#define PLATFORM_GPS_TX_PIN                   (GPIO_NUM_23)