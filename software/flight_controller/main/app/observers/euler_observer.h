#pragma once

#include <utils/madgwick.h>

class EulerObserver : public Madgwick
{
  public:

    /* Constructors */

    EulerObserver(float period);
};