#pragma once

#include <utils/matrix.h>

using namespace std;

class Vect : public Matrix
{
  private:

    float inv_sqrt(float);

  public:

    Vect(unsigned int, double initial = 0.0);
    Vect(Matrix mat);

    void set(unsigned int, float);

    float operator()(const unsigned &);

    float norm(void);

    void normalize();
};