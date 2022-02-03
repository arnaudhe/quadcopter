#pragma once

#include <stdint.h>
#include <stddef.h>

#define BMP280_SPI_INTF                      (0)
#define BMP280_I2C_INTF                      (1)

/*! @name Chip IDs for samples and mass production parts */
#define BMP280_CHIP_ID1                      (0x56)
#define BMP280_CHIP_ID2                      (0x57)
#define BMP280_CHIP_ID3                      (0x58)
#define BMP280_CHIP_BME                      (0x60)

/*! @name I2C addresses */
#define BMP280_I2C_ADDR_PRIM                 (0x76)
#define BMP280_I2C_ADDR_SEC                  (0x77)

/*! @name Calibration parameter register addresses*/
#define BMP280_DIG_T1_LSB_ADDR               (0x88)
#define BMP280_DIG_T1_MSB_ADDR               (0x89)
#define BMP280_DIG_T2_LSB_ADDR               (0x8A)
#define BMP280_DIG_T2_MSB_ADDR               (0x8B)
#define BMP280_DIG_T3_LSB_ADDR               (0x8C)
#define BMP280_DIG_T3_MSB_ADDR               (0x8D)
#define BMP280_DIG_P1_LSB_ADDR               (0x8E)
#define BMP280_DIG_P1_MSB_ADDR               (0x8F)
#define BMP280_DIG_P2_LSB_ADDR               (0x90)
#define BMP280_DIG_P2_MSB_ADDR               (0x91)
#define BMP280_DIG_P3_LSB_ADDR               (0x92)
#define BMP280_DIG_P3_MSB_ADDR               (0x93)
#define BMP280_DIG_P4_LSB_ADDR               (0x94)
#define BMP280_DIG_P4_MSB_ADDR               (0x95)
#define BMP280_DIG_P5_LSB_ADDR               (0x96)
#define BMP280_DIG_P5_MSB_ADDR               (0x97)
#define BMP280_DIG_P6_LSB_ADDR               (0x98)
#define BMP280_DIG_P6_MSB_ADDR               (0x99)
#define BMP280_DIG_P7_LSB_ADDR               (0x9A)
#define BMP280_DIG_P7_MSB_ADDR               (0x9B)
#define BMP280_DIG_P8_LSB_ADDR               (0x9C)
#define BMP280_DIG_P8_MSB_ADDR               (0x9D)
#define BMP280_DIG_P9_LSB_ADDR               (0x9E)
#define BMP280_DIG_P9_MSB_ADDR               (0x9F)

/*! @name Other registers */
#define BMP280_CHIP_ID_ADDR                  (0xD0)
#define BMP280_SOFT_RESET_ADDR               (0xE0)
#define BMP280_STATUS_ADDR                   (0xF3)
#define BMP280_CTRL_MEAS_ADDR                (0xF4)
#define BMP280_CONFIG_ADDR                   (0xF5)
#define BMP280_PRES_MSB_ADDR                 (0xF7)
#define BMP280_PRES_LSB_ADDR                 (0xF8)
#define BMP280_PRES_XLSB_ADDR                (0xF9)
#define BMP280_TEMP_MSB_ADDR                 (0xFA)
#define BMP280_TEMP_LSB_ADDR                 (0xFB)
#define BMP280_TEMP_XLSB_ADDR                (0xFC)

/*! @name Power modes */
#define BMP280_SLEEP_MODE                    (0x00)
#define BMP280_FORCED_MODE                   (0x01)
#define BMP280_NORMAL_MODE                   (0x03)

/*! @name Soft reset command */
#define BMP280_SOFT_RESET_CMD                (0xB6)

/*! @name ODR options */
#define BMP280_ODR_0_5_MS                    (0x00)
#define BMP280_ODR_62_5_MS                   (0x01)
#define BMP280_ODR_125_MS                    (0x02)
#define BMP280_ODR_250_MS                    (0x03)
#define BMP280_ODR_500_MS                    (0x04)
#define BMP280_ODR_1000_MS                   (0x05)
#define BMP280_ODR_2000_MS                   (0x06)
#define BMP280_ODR_4000_MS                   (0x07)

/*! @name Over-sampling macros */
#define BMP280_OS_NONE                       (0x00)
#define BMP280_OS_1X                         (0x01)
#define BMP280_OS_2X                         (0x02)
#define BMP280_OS_4X                         (0x03)
#define BMP280_OS_8X                         (0x04)
#define BMP280_OS_16X                        (0x05)

/*! @name Filter coefficient macros */
#define BMP280_FILTER_OFF                    (0x00)
#define BMP280_FILTER_COEFF_2                (0x01)
#define BMP280_FILTER_COEFF_4                (0x02)
#define BMP280_FILTER_COEFF_8                (0x03)
#define BMP280_FILTER_COEFF_16               (0x04)

/*! @name SPI 3-Wire macros */
#define BMP280_SPI3_WIRE_ENABLE              (1)
#define BMP280_SPI3_WIRE_DISABLE             (0)

/*! @name Measurement status */
#define BMP280_MEAS_DONE                     (0)
#define BMP280_MEAS_ONGOING                  (1)

/*! @name Image update */
#define BMP280_IM_UPDATE_DONE                (0)
#define BMP280_IM_UPDATE_ONGOING             (1)

/*! @name Position and mask macros */
#define BMP280_STATUS_IM_UPDATE_POS          (0)
#define BMP280_STATUS_IM_UPDATE_MASK         (0x01)
#define BMP280_STATUS_MEAS_POS               (3)
#define BMP280_STATUS_MEAS_MASK              (0x08)
#define BMP280_OS_TEMP_POS                   (5)
#define BMP280_OS_TEMP_MASK                  (0xE0)
#define BMP280_OS_PRES_POS                   (2)
#define BMP280_OS_PRES_MASK                  (0x1C)
#define BMP280_POWER_MODE_POS                (0)
#define BMP280_POWER_MODE_MASK               (0x03)
#define BMP280_STANDBY_DURN_POS              (5)
#define BMP280_STANDBY_DURN_MASK             (0xE0)
#define BMP280_FILTER_POS                    (2)
#define BMP280_FILTER_MASK                   (0x1C)
#define BMP280_SPI3_ENABLE_POS               (0)
#define BMP280_SPI3_ENABLE_MASK              (0x01)

/*! @name Calibration parameters' relative position */
#define BMP280_DIG_T1_LSB_POS                (0)
#define BMP280_DIG_T1_MSB_POS                (1)
#define BMP280_DIG_T2_LSB_POS                (2)
#define BMP280_DIG_T2_MSB_POS                (3)
#define BMP280_DIG_T3_LSB_POS                (4)
#define BMP280_DIG_T3_MSB_POS                (5)
#define BMP280_DIG_P1_LSB_POS                (6)
#define BMP280_DIG_P1_MSB_POS                (7)
#define BMP280_DIG_P2_LSB_POS                (8)
#define BMP280_DIG_P2_MSB_POS                (9)
#define BMP280_DIG_P3_LSB_POS                (10)
#define BMP280_DIG_P3_MSB_POS                (11)
#define BMP280_DIG_P4_LSB_POS                (12)
#define BMP280_DIG_P4_MSB_POS                (13)
#define BMP280_DIG_P5_LSB_POS                (14)
#define BMP280_DIG_P5_MSB_POS                (15)
#define BMP280_DIG_P6_LSB_POS                (16)
#define BMP280_DIG_P6_MSB_POS                (17)
#define BMP280_DIG_P7_LSB_POS                (18)
#define BMP280_DIG_P7_MSB_POS                (19)
#define BMP280_DIG_P8_LSB_POS                (20)
#define BMP280_DIG_P8_MSB_POS                (21)
#define BMP280_DIG_P9_LSB_POS                (22)
#define BMP280_DIG_P9_MSB_POS                (23)
#define BMP280_CALIB_DATA_SIZE               (24)

/*! @brief Macros holding the minimum and maximum for trimming values */

#define BMP280_ST_DIG_T1_MIN                (19000)
#define BMP280_ST_DIG_T1_MAX                (35000)
#define BMP280_ST_DIG_T2_MIN                (22000)
#define BMP280_ST_DIG_T2_MAX                (30000)
#define BMP280_ST_DIG_T3_MIN                (-3000)
#define BMP280_ST_DIG_T3_MAX                (-1000)
#define BMP280_ST_DIG_P1_MIN                (30000)
#define BMP280_ST_DIG_P1_MAX                (42000)
#define BMP280_ST_DIG_P2_MIN                (-12970)
#define BMP280_ST_DIG_P2_MAX                (-8000)
#define BMP280_ST_DIG_P3_MIN                (-5000)
#define BMP280_ST_DIG_P3_MAX                (8000)
#define BMP280_ST_DIG_P4_MIN                (-10000)
#define BMP280_ST_DIG_P4_MAX                (18000)
#define BMP280_ST_DIG_P5_MIN                (-500)
#define BMP280_ST_DIG_P5_MAX                (1100)
#define BMP280_ST_DIG_P6_MIN                (-1000)
#define BMP280_ST_DIG_P6_MAX                (1000)
#define BMP280_ST_DIG_P7_MIN                (-32768)
#define BMP280_ST_DIG_P7_MAX                (32767)
#define BMP280_ST_DIG_P8_MIN                (-30000)
#define BMP280_ST_DIG_P8_MAX                (10000)
#define BMP280_ST_DIG_P9_MIN                (-10000)
#define BMP280_ST_DIG_P9_MAX                (30000)

/*! @brief Macros to read out API revision number */
/* Register holding custom trimming values */
#define BMP280_ST_TRIMCUSTOM_REG             (0x87)
#define BMP280_ST_TRIMCUSTOM_REG_APIREV__POS (1)
#define BMP280_ST_TRIMCUSTOM_REG_APIREV__MSK (0x06)
#define BMP280_ST_TRIMCUSTOM_REG_APIREV__LEN (2)
#define BMP280_ST_TRIMCUSTOM_REG_APIREV__REG BMP280_ST_TRIMCUSTOM_REG

/* highest API revision supported is revision 0. */
#define BMP280_ST_MAX_APIREVISION            (0x00)

/*! @brief Macros holding the minimum and maximum for trimming values */
/* 0x00000 is minimum output value */
#define BMP280_ST_ADC_T_MIN                  (0x00000)

/* 0xFFFF0 is maximum 20-bit output value without over sampling */
#define BMP280_ST_ADC_T_MAX                  (0xFFFF0)

/* 0x00000 is minimum output value */
#define BMP280_ST_ADC_P_MIN                  (0x00000)

/* 0xFFFF0 is maximum 20-bit output value without over sampling */
#define BMP280_ST_ADC_P_MAX                  (0xFFFF0)
