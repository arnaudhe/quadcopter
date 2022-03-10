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
#define MOTORS_CONTROLLER_PERIOD              (0.05)
#define HEIGHT_CONTROLLER_PERIOD              (0.025)
#define POSITION_CONTROLLER_PERIOD            (0.1)
#define BATTERY_SUPERVISOR_PERIOD             (5.0)
#define CAMERA_SUPERVISOR_PERIOD              (0.05)
#define HEARTBEAT_PERIOD                      (1.0)
#define RADIO_PERIOD                          (0.02)

#define PLATFORM_RADIO_ADDRESS                (31)


#define PLATFORM_HCSR04_ECHO_PIN              (5)
#define PLATFORM_HCSR04_TRIG_CHANNEL          (4)
#define PLATFORM_HCSR04_TRIG_PIN              (4)

#define PLATFORM_GPS_UART                     (UART_NUM_1)
#define PLATFORM_GPS_RX_PIN                   (GPIO_NUM_22)
#define PLATFORM_GPS_TX_PIN                   (GPIO_NUM_23)

#define PLATFORM_BAROMETER_MODEL              BMP280

#define PLATFORM_SPI_NUM                      (SPI2_HOST)
#define PLATFORM_SPI_CLK_PIN                  (25)
#define PLATFORM_SPI_MISO_PIN                 (26)
#define PLATFORM_SPI_MOSI_PIN                 (27)

#define PLATFORM_SI4432_CS_PIN                (32)
#define PLATFORM_SI4432_SPI_FREQ              (1000000)
#define PLATFORM_SI4432_SPI_MODE              (0)
#define PLATFORM_SI4432_IRQ_PIN               (GPIO_NUM_35)

#define PLATFORM_ADC_VREF                     (1075)

#define PLATFORM_BATTERY_ADC_UNIT             (Adc::UNIT_1)
#define PLATFORM_BATTERY_ADC_CHANNEL          (Adc::CHANNEL_6)
#define PLATFORM_BATTERY_CELLS                (3)
#define PLATFORM_BATTERY_DIVIDER              (6.13f)
