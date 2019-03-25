#include <drv/BMP180.h>
#include <drv/BMP180_defs.h>
#include <drv/BMP180_conf.h>
#include <math.h>

BMP180::BMP180(I2cMaster * i2c)
{
    _i2c = i2c;
}

esp_err_t BMP180::init()
{
    struct bmp180_calib_param_t calib; 
	double    c3, c4, b1;
    esp_err_t ret;

	/* Retrieve calibration data from device */
    ret = _i2c->read(BMP180_I2C_ADDR, BMP180_PROM_START__ADDR, (uint8_t *)&calib, BMP180_PROM_DATA__LEN);

    if (ret == ESP_OK)
    {
        /* Compute doubleing-point polynominals */
        c3 = 160.0 * pow(2,-15) * calib.ac3;
        c4 = pow(10,-3) * pow(2,-15) * calib.ac4;
        b1 = pow(160,2) * pow(2,-30) * calib.b1;
        c5 = (pow(2,-15) / 160) * calib.ac5;
        c6 = calib.ac6;
        mc = (pow(2,11) / pow(160,2)) * calib.mc;
        md = calib.md / 160.0;
        x0 = calib.ac1;
        x1 = 160.0 * pow(2,-13) * calib.ac2;
        x2 = pow(160,2) * pow(2,-25) * calib.b2;
        y0 = c4 * pow(2,15);
        y1 = c4 * c3;
        y2 = c4 * b1;
        p0 = (3791.0 - 8.0) / 1600.0;
        p1 = 1.0 - 7357.0 * pow(2,-20);
        p2 = 3038.0 * 100.0 * pow(2,-36);
    }

    return ret;
}

esp_err_t BMP180::start_temperature(void)
{
    return _i2c->write_register(BMP180_I2C_ADDR, BMP180_CTRL_MEAS_REG, BMP180_T_MEASURE);
}

esp_err_t BMP180::start_pressure(void)
{
    return _i2c->write_register(BMP180_I2C_ADDR, BMP180_CTRL_MEAS_REG, BMP180_P_MEASURE | (BMP180_CONF_OVERSAMPLING << 6));
}

esp_err_t BMP180::get_temperature(double &T)
{
	esp_err_t     ret;
    uint16_t      raw;
	double        tu, a;
	
	ret = _i2c->read_uint(BMP180_I2C_ADDR, BMP180_ADC_OUT_MSB_REG, &raw);

	if (ret == ESP_OK) // good read, calculate temperature
	{
		tu = (double)raw;
		a  = c5 * (tu - c6);
		T  = a + (mc / (a + md));
        _temperature = T;
	}

	return ret;
}

esp_err_t BMP180::get_pressure(double &P)
{
	uint8_t       data[BMP180_PRESSURE_DATA_BYTES];
	esp_err_t     ret;
	double        pu,s,x,y,z;

	ret = _i2c->read(BMP180_I2C_ADDR, BMP180_ADC_OUT_MSB_REG, data, BMP180_PRESSURE_DATA_BYTES);

	if (ret) // good read, calculate pressure
	{
		pu = (data[0] * 256.0) + data[1] + (data[2]/256.0);

		s = _temperature - 25.0;
		x = (x2 * pow(s,2)) + (x1 * s) + x0;
		y = (y2 * pow(s,2)) + (y1 * s) + y0;
		z = (pu - x) / y;
		P = (p2 * pow(z,2)) + (p1 * z) + p0;
	}

	return ret;
}

double BMP180::sea_level(double P, double A)
{
	return (P / pow(1 - (A / 44330.0), 5.255));
}

double BMP180::altitude(double P, double P0)
{
	return (44330.0 * (1 - pow(P / P0, 1 / 5.255)));
}
