#include <utils/height_observer.h>
#include <utils/height_observer_conf.h>

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

    _process_noise_matrix.set(0, 0, HEIGHT_OBSERVER_STATE_HEIGHT_COVARIANCE);
    _process_noise_matrix.set(1, 1, HEIGHT_OBSERVER_STATE_SPEED_COVARIANCE);

    _measure_noise_matrix.set(0, 0, HEIGHT_OBSERVER_SENSOR_BAROMETER_COVARIANCE);
    _measure_noise_matrix.set(1, 1, HEIGHT_OBSERVER_SENSOR_ULTRASOUND_COVARIANCE);
}

void HeightObserver::update(float acc_z, float baro, float ultrasound)
{
    Matrix input(1, 1, acc_z);
    Matrix output(2, 1);

    output.set(0, 0, baro);
    output.set(1, 0, ultrasound);

    Kalman::update(input, output);
}

float HeightObserver::height(void)
{
    return _state(0, 0);
}

float HeightObserver::vertical_speed(void)
{
    return _state(1, 0);
}
