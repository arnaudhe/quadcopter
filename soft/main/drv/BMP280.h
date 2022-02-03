#pragma once

#include "BMP280_defs.h"

#include <esp_err.h>
#include <periph/i2c_master.h>

class BMP280
{

private:

    struct calib_param
    {
        uint16_t dig_t1;
        int16_t  dig_t2;
        int16_t  dig_t3;
        uint16_t dig_p1;
        int16_t  dig_p2;
        int16_t  dig_p3;
        int16_t  dig_p4;
        int16_t  dig_p5;
        int16_t  dig_p6;
        int16_t  dig_p7;
        int16_t  dig_p8;
        int16_t  dig_p9;
        int32_t  t_fine;
    };

    struct status
    {
        uint8_t measuring;
        uint8_t im_update;
    };

    I2cMaster           * _i2c;              ///< i2c device
    BMP280::calib_param   _calib_param;
    uint8_t               _chip_id;

    /**
     * @brief Load calibration value from device. Those values are used to calculate
     * compensated pressure and temperature
     *
     * @return API execution status
     */
    esp_err_t _get_calib(void);

    /**
     * @brief Send config to device after power up
     *
     * @return API execution status
     */
    esp_err_t _write_config(void);

    /**
    * @brief This API is used to get the compensated temperature from
    * uncompensated temperature. This API uses double floating precision.
    * Temperature in degree Celsius , double precision. output value
    * of "51.23" equals 51.23 degC.
    *
    * @return compensated temperature
    */
    double _get_compensated_temperature(int32_t uncomp_temperature);

    /**
    * @brief This API is used to get the compensated pressure from
    * uncompensated pressure. This API uses double floating precision.
    * Pressure in Pa as double. Output value of "96386.2"
    * equals 96386.2 Pa = 963.862 hPa
    *
    * @return compensated pressure
    */
    double _get_compensated_pressure(int32_t uncomp_pressure);

public:

    BMP280(I2cMaster * i2c);

    /**
    * @brief This API triggers the soft reset of the sensor.
    * @return Result of API execution
    */
    esp_err_t soft_reset(void);

    /**
    *  @brief This API is the entry point.
    *  It reads the chip-id and calibration data from the sensor.
    *
    *  @return Result of API execution
    */
    esp_err_t init(void);

    /**
    * @brief This API reads the status register
    *
    * @param[out] measuring : BMP280_MEAS_DONE, BMP280_MEAS_ONGOING
    * @param[out] im_update : BMP280_IM_UPDATE_DONE, BMP280_IM_UPDATE_ONGOING
    *
    * @return Result of API execution
    */
    esp_err_t get_status(bool * measuring, bool * im_update);

    /**
    * @brief This API reads the temperature and pressure data registers.
    *
    * @param[out] temperature in °C
    * @param[out] pressure in Pa
    *
    * @return Result of API execution
    */
    esp_err_t read_temperature_pressure(double * temperature, double * pressure);

    /**
    * @brief This API computes the measurement time in milliseconds for the
    * active configuration
    *
    * @return Measurement time for the active configuration in milliseconds
    */
    int get_measure_time();
};
