#include <utils/height_observer.h>
#include <math.h>

#define HEIGHT_OBSERVER_GAIN      0.01f

HeightObserver::HeightObserver(float period) :
    _state(2, 1),
    _state_matrix(2, 2),
    _input_matrix(2, 1),
    _output_matrix(1, 2),
    _gain_matrix(2, 1)
{
    _period         = period;
    _gain           = HEIGHT_OBSERVER_GAIN;
    _height         = 0.0f;
    _vertical_speed = 0.0f;

    _state_matrix.set(0, 0, 1.0);
    _state_matrix.set(0, 1, period);
    _state_matrix.set(1, 0, 0.0);
    _state_matrix.set(1, 1, 1.0);

    _input_matrix.set(0, 0, period * period / 2.0);
    _input_matrix.set(1, 0, period);

    _output_matrix.set(0, 0, 1.0);
    _output_matrix.set(0, 1, 0.0);

    _gain_matrix.set(0, 0, HEIGHT_OBSERVER_GAIN);
    _gain_matrix.set(1, 0, period);

    _state.display("state");

}

void HeightObserver::update(float acc_z, float baro)
{
    _state = (_state_matrix - (_gain_matrix * _output_matrix)) * _state + _input_matrix * acc_z + _gain_matrix * baro;
}

float HeightObserver::height(void)
{
    return _state(0, 0);
}

float HeightObserver::vertical_speed(void)
{
    return _state(1, 0);
}
