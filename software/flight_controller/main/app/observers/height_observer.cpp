#include <app/observers/height_observer.h>
#include <app/observers/height_observer_conf.h>
#include <math.h>
#include <hal/log.h>

#define ULTRASOUND_MAX_RANGE_m              3.0f

HeightObserver::HeightObserver(float period):
    Kalman(period, 2, 1, 2)
{
    _state_matrix.set(0, 0, 1.0);
    _state_matrix.set(0, 1, period);
    _state_matrix.set(1, 0, 0.0);
    _state_matrix.set(1, 1, 1.0);

    _input_matrix.set(0, 0, period * period / 2.0);
    _input_matrix.set(1, 0, period);

    _output_matrix.set(0, 0, 1.0);
    _output_matrix.set(0, 1, 0.0);
    _output_matrix.set(1, 0, 0.0);
    _output_matrix.set(1, 1, 1.0);

    _process_noise_matrix.set(0, 0, powf(period, 4) / 4 * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION);
    _process_noise_matrix.set(0, 1, powf(period, 3) / 2 * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION);
    _process_noise_matrix.set(1, 0, powf(period, 3) / 2 * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION);
    _process_noise_matrix.set(1, 1, powf(period, 2) * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION);

    _measure_noise_matrix.set(0, 0, HEIGHT_OBSERVER_SENSOR_BAROMETER_DEVIATION * HEIGHT_OBSERVER_SENSOR_BAROMETER_DEVIATION);
    _measure_noise_matrix.set(1, 1, HEIGHT_OBSERVER_ZERO_VELOCITY_DEVIATION * HEIGHT_OBSERVER_ZERO_VELOCITY_DEVIATION);

    _covariance.set(0, 0, 0.01);
    _covariance.set(1, 1, 0.01);
}

void HeightObserver::update(float acc_z, float baro, float ultrasound)
{
    Matrix input(1, 1, acc_z - 0.15f);
    Matrix measure(2, 1, 0.0f);

    measure.set(0, 0, baro);
    measure.set(1, 0, 0.0);

    if (fabs(acc_z - 0.15f) < 0.2f)
    {
        _output_matrix.set(1, 1, 1.0);
    }
    else
    {
        _output_matrix.set(1, 1, 0.0);
    }

    Kalman::update(input, measure);
}

float HeightObserver::height(void)
{
    return _state(0, 0);
}

float HeightObserver::vertical_speed(void)
{
    return _state(1, 0);
}
