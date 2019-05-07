#pragma once

#include <utils/matrix.h>

using namespace std;

class Vect : public Matrix
{
  private:

    double inv_sqrt(double);

  public:

    Vect(unsigned int, double initial = 0.0);
    Vect(Matrix mat);

    void set(unsigned int, double);

    double operator()(const unsigned &);

    double norm(void);

    void normalize();
};