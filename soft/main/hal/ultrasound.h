#pragma once

#include <drv/HCSR04.h>

class UltrasoundSensor
{

  private:

    HcSr04 * _hc;

  public:

    UltrasoundSensor();

    float height();
};