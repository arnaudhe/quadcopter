#include <app/observers/position_observer.h>
#include <app/observers/position_observer_conf.h>
#include <math.h>
#include <hal/log.h>

#define ULTRASOUND_MAX_RANGE_m              3.0f

PositionObserver::PositionObserver(float period):
    Kalman(period, 4, 2, 2)
{
    _state_matrix.set(0, 0, 1.0);
    _state_matrix.set(0, 1, period);
    _state_matrix.set(0, 2, 0.0);
    _state_matrix.set(0, 3, 0.0);
    _state_matrix.set(1, 0, 0.0);
    _state_matrix.set(1, 1, 1.0);
    _state_matrix.set(1, 2, 0.0);
    _state_matrix.set(1, 3, 0.0);
    _state_matrix.set(2, 0, 0.0);
    _state_matrix.set(2, 1, 0.0);
    _state_matrix.set(2, 2, 1.0);
    _state_matrix.set(2, 3, period);
    _state_matrix.set(3, 0, 0.0);
    _state_matrix.set(3, 1, 0.0);
    _state_matrix.set(3, 2, 0.0);
    _state_matrix.set(3, 3, 1.0);

    _input_matrix.set(0, 0, period * period / 2.0);
    _input_matrix.set(1, 0, period);
    _input_matrix.set(2, 0, 0.0f);
    _input_matrix.set(3, 0, 0.0f);
    _input_matrix.set(0, 1, 0.0f);
    _input_matrix.set(1, 1, 0.0f);
    _input_matrix.set(2, 1, period * period / 2.0);
    _input_matrix.set(3, 1, period);

    _output_matrix.set(0, 0, 1.0f);
    _output_matrix.set(0, 1, 0.0f);
    _output_matrix.set(0, 2, 0.0f);
    _output_matrix.set(0, 3, 0.0f);
    _output_matrix.set(1, 0, 0.0f);
    _output_matrix.set(1, 1, 0.0f);
    _output_matrix.set(1, 2, 1.0f);
    _output_matrix.set(1, 3, 0.0f);

    _state_matrix.display("state");
    _input_matrix.display("input");
    _output_matrix.display("output");

    _process_noise_matrix.set(0, 0, powf(period, 4) / 4 * POSITION_OBSERVER_ACCELEROMETER_DEVIATION * POSITION_OBSERVER_ACCELEROMETER_DEVIATION);
    _process_noise_matrix.set(0, 1, powf(period, 3) / 2 * POSITION_OBSERVER_ACCELEROMETER_DEVIATION * POSITION_OBSERVER_ACCELEROMETER_DEVIATION);
    _process_noise_matrix.set(1, 0, powf(period, 3) / 2 * POSITION_OBSERVER_ACCELEROMETER_DEVIATION * POSITION_OBSERVER_ACCELEROMETER_DEVIATION);
    _process_noise_matrix.set(1, 1, powf(period, 2) * POSITION_OBSERVER_ACCELEROMETER_DEVIATION * POSITION_OBSERVER_ACCELEROMETER_DEVIATION);
    _process_noise_matrix.set(2, 2, powf(period, 4) / 4 * POSITION_OBSERVER_ACCELEROMETER_DEVIATION * POSITION_OBSERVER_ACCELEROMETER_DEVIATION);
    _process_noise_matrix.set(2, 3, powf(period, 3) / 2 * POSITION_OBSERVER_ACCELEROMETER_DEVIATION * POSITION_OBSERVER_ACCELEROMETER_DEVIATION);
    _process_noise_matrix.set(3, 2, powf(period, 3) / 2 * POSITION_OBSERVER_ACCELEROMETER_DEVIATION * POSITION_OBSERVER_ACCELEROMETER_DEVIATION);
    _process_noise_matrix.set(3, 3, powf(period, 2) * POSITION_OBSERVER_ACCELEROMETER_DEVIATION * POSITION_OBSERVER_ACCELEROMETER_DEVIATION);

    _process_noise_matrix.display("process noise");

    _measure_noise_matrix.set(0, 0, POSITION_OBSERVER_SENSOR_GPS_DEVIATION * POSITION_OBSERVER_SENSOR_GPS_DEVIATION);
    _measure_noise_matrix.set(1, 1, POSITION_OBSERVER_SENSOR_GPS_DEVIATION * POSITION_OBSERVER_SENSOR_GPS_DEVIATION);

    _measure_noise_matrix.display("measure noise");

    _covariance.set(0, 0, 0.01);
    _covariance.set(1, 1, 0.01);
}

void PositionObserver::update(float acc_x, float acc_y, float gps_x, float gps_y)
{
    Matrix input(2, 1);
    Matrix measure(2, 1);

    input.set(0, 0, acc_x);
    input.set(1, 0, acc_x);

    measure.set(0, 0, gps_x);
    measure.set(1, 0, gps_y);

    Kalman::update(input, measure);
}

float PositionObserver::x(void)
{
    return _state(0, 0);
}

float PositionObserver::x_speed(void)
{
    return _state(1, 0);
}

float PositionObserver::y(void)
{
    return _state(2, 0);
}

float PositionObserver::y_speed(void)
{
    return _state(3, 0);
}
