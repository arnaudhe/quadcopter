#include <utils/kalman.h>
#include <esp_attr.h>

Kalman::Kalman(float period, int state_dim, int input_dim, int output_dim):
    _period(period),
    _state_matrix(state_dim, state_dim),
    _input_matrix(state_dim, input_dim),
    _output_matrix(output_dim, state_dim),
    _process_noise_matrix(state_dim, state_dim),
    _measure_noise_matrix(output_dim, output_dim),
    _state(state_dim, 1),
    _covariance(state_dim, state_dim)
{
}

Kalman::Kalman(float period, Matrix A, Matrix B, Matrix C, Matrix Q, Matrix R, Matrix x_initial):
    _period(period),
    _state_matrix(A),
    _input_matrix(B),
    _output_matrix(C),
    _process_noise_matrix(Q),
    _measure_noise_matrix(R),
    _state(x_initial),
    _covariance(A.rows(), A.rows())
{
}

void IRAM_ATTR Kalman::update(Matrix input, Matrix measure)
{
    /* Prediction */
    _state = _state_matrix * _state + _input_matrix * input;
    _covariance = _state_matrix * _covariance * _state_matrix.transpose() + _process_noise_matrix;

    /* update */
    Matrix measure_error = measure - _output_matrix * _state;
    Matrix innovation_covar = _output_matrix * _covariance * _output_matrix.transpose() + _measure_noise_matrix;
    Matrix gain = _covariance * _output_matrix.transpose() * innovation_covar.invert();

    _state = _state + gain * measure_error;
    _covariance = (Matrix::identity(_output_matrix.columns()) - gain * _output_matrix) * _covariance;
}

Matrix Kalman::state(void)
{
    return _state;
}