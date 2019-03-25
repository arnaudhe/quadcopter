#pragma once

#include <utils/matrix.h>

class Kalman
{

  protected:

    float  _period;
    Matrix _state_matrix;           /* State evolution matrix */
    Matrix _input_matrix;           /* Input matrix */
    Matrix _output_matrix;          /* Output matrix */
    Matrix _process_noise_matrix;   /* State covariance matrix */
    Matrix _measure_noise_matrix;   /* Output measure covariance matrix */

    Matrix _state;   /* Current state */
    Matrix _covariance;

  public:

    Kalman(float period, int state_dim, int input_dim, int output_dim);
    Kalman(float period, Matrix A, Matrix B, Matrix C, Matrix Q, Matrix R, Matrix x_initial);

    void update(Matrix u, Matrix y);

    Matrix state(void);

};