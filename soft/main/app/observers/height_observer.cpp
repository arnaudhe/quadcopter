#include <app/observers/height_observer.h>
#include <app/observers/height_observer_conf.h>
#include <math.h>
#include <stdio.h>

#define ZERO_VELOCITY_DETECTOR_STEPS        10
#define ZERO_VELOCITY_DETECTOR_THRESHOLD    0.1

HeightObserver::HeightObserver(float period):
    Kalman(period, 2, 1, 3)
{
    _zero_velocity_detect_count = ZERO_VELOCITY_DETECTOR_STEPS;

    _state_matrix.set(0, 0, 1.0);
    _state_matrix.set(0, 1, 5.0 * period);
    _state_matrix.set(1, 0, 0.0);
    _state_matrix.set(1, 1, 1.0);

    _input_matrix.set(0, 0, 5.0 * period * period / 2.0);
    _input_matrix.set(1, 0, 5.0 * period);

    _output_matrix.set(0, 0, 1.0);
    _output_matrix.set(0, 1, 0.0);
    _output_matrix.set(1, 0, 1.0);
    _output_matrix.set(1, 1, 0.0);
    _output_matrix.set(2, 0, 0.0);
    _output_matrix.set(2, 1, 1.0);

    _process_noise_matrix.set(0, 0, powf(period, 4) / 4 * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION);
    _process_noise_matrix.set(0, 1, powf(period, 3) / 2 * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION);
    _process_noise_matrix.set(1, 0, powf(period, 3) / 2 * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION);
    _process_noise_matrix.set(1, 1, powf(period, 4) * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION * HEIGHT_OBSERVER_ACCELEROMETER_DEVIATION);

    _measure_noise_matrix.set(0, 0, HEIGHT_OBSERVER_SENSOR_BAROMETER_DEVIATION * HEIGHT_OBSERVER_SENSOR_BAROMETER_DEVIATION);
    _measure_noise_matrix.set(1, 1, HEIGHT_OBSERVER_SENSOR_ULTRASOUND_DEVIATION * HEIGHT_OBSERVER_SENSOR_ULTRASOUND_DEVIATION);     // Speed measure covariance
    _measure_noise_matrix.set(2, 2, 0.0001);     // Speed measure covariance

    _covariance.set(0, 0, 0.01);
    _covariance.set(1, 1, 0.01);
    _covariance.set(1, 1, 0.01);
}

void HeightObserver::update(float acc_z, float baro, float ultrasound)
{
    Matrix input(1, 1, acc_z);
    Matrix measure(3, 1);

    if (fabsf(acc_z) < ZERO_VELOCITY_DETECTOR_THRESHOLD)
    {
        if (_zero_velocity_detect_count < ZERO_VELOCITY_DETECTOR_STEPS)
        {
            _zero_velocity_detect_count++;
            if (_zero_velocity_detect_count == ZERO_VELOCITY_DETECTOR_STEPS)
            {
                printf("Zero speed\n");
            }
        }
    }
    else
    {
        if (_zero_velocity_detect_count == ZERO_VELOCITY_DETECTOR_STEPS)
        {
            printf("Moving\n");
        }
        _zero_velocity_detect_count = 0;
    }

    if (_zero_velocity_detect_count == ZERO_VELOCITY_DETECTOR_STEPS)
    {
        measure.set(0, 0, baro);
        measure.set(1, 0, ultrasound);
        measure.set(2, 0, 0.0);
    }
    else
    {
        measure.set(0, 0, baro);
        measure.set(1, 0, ultrasound);
        measure.set(2, 0, _state(1, 0));
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
