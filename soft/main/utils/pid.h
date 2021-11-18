#pragma once

#include <iostream>

#include <utils/filter.h>

using namespace std;

class Pid
{
  
  private: 

    /* Attributes */

    float      _kp;                 ///< proportionnal coeff
    float      _ki;                 ///< integrate coeff
    float      _kd;                 ///< derivative coeff
    float      _kff;                ///< feed-forward coeff
    float      _setpoint;           ///< current regulator setpoint
    float      _previous;           ///< previous input, for derivate
    float      _integrate;          ///< accumalator, for integrate
    float      _period;             ///< time period
    Filter   * _dterm_filter;       ///< low-pass filter to reduce d-term noise

  public:

    /* Constructors */

    Pid(float _period, float kp, float ki, float kd, float kff = 0.0f);

    /* Accessors*/

    void set_kp(float kp);
    void set_ki(float ki);
    void set_kd(float kd);
    void set_kff(float kff);
    void set_setpoint(float setpoint);

    float kp() const;
    float ki() const;
    float kd() const;
    float kff() const;
    float setpoint() const;

    /* Other methods */

    float update(float input);

};