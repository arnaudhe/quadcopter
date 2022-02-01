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
    float      _kt;                 ///< anti-windup coeff
    float      _setpoint;           ///< current regulator setpoint
    float      _previous;           ///< previous input, for derivate
    float      _integrate;          ///< accumalator, for integrate
    float      _windup;             ///< integrate windup accumulation
    float      _period;             ///< time period
    Filter   * _dterm_filter;       ///< low-pass filter to reduce d-term noise
    float      _min_command;        ///< output command low saturation
    float      _max_command;        ///< output command heigh saturation

  public:

    /* Constructors */

    Pid(float period, float kp, float ki, float kd, float kff = 0.0f, float kt = 0.0f,
        float min_command = -0.1f, float max_command = 0.1f);

    /* Accessors*/

    void set_kp(float kp);
    void set_ki(float ki);
    void set_kd(float kd);
    void set_kff(float kff);
    void set_kt(float kt);
    void set_setpoint(float setpoint);

    float kp() const;
    float ki() const;
    float kd() const;
    float kff() const;
    float kt() const;
    float setpoint() const;

    /* Other methods */

    float update(float input);
    void reset(void);

};