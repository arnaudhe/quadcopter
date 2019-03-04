#pragma once

#include <utils/matrix.h>

using namespace std;

class HeightObserver
{

  private: 

    /* Attributes */

    float _period;
    float _gain;     ///< observer gain

    float _height;
    float _vertical_speed;

    Matrix _state;
    Matrix _state_matrix;
    Matrix _input_matrix;
    Matrix _output_matrix;
    Matrix _gain_matrix;

    /* Methods */

  public:

    /* Constructors */

    HeightObserver(float period);

    /* Accessors*/

    float height(void);
    float vertical_speed(void);

    /* Other methods */

    void update(float acc_z, float baro);

};
