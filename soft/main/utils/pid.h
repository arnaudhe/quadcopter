#pragma once

#include <iostream>

using namespace std;

class Pid
{
  
  private: 

    /* Attributes */

    float _kp;        ///< proportionnal coeff
    float _ki;        ///< integrate coeff
    float _kd;        ///< derivative coeff
    float _consign;   ///< current regulator consign
    float _previous;  ///< previous input, for derivate
    float _integrate; ///< accumalator, for integrate
    float _period;    ///< time period

  public:

    /* Constructors */

    Pid(float _period, float kp, float ki, float kd);

    /* Accessors*/

    void set_kp(float kp);
    void set_ki(float ki);
    void set_kd(float kd);
    void set_consign(float consign);

    float kp() const;
    float ki() const;
    float kd() const;
    float consign() const;

    /* Other methods */

    float update(float input);

};