#include <utils/vector.h>
#include <math.h>

Vect::Vect(unsigned int dim, double initial):
    Matrix(dim, 1, initial)
{
}

Vect::Vect(Matrix mat):
    Matrix(mat.rows(), 1)
{
    set_data(mat.data());
}

void Vect::set(unsigned int index, double value)
{
    _data[index][0] = value;
}

double Vect::operator()(const unsigned &index)
{
    return _data[index][0];
}

double Vect::norm(void)
{
    unsigned int i;
    double norm = 0.0f;

    for (i = 0; i < _rows; i++)
    {
        norm += _data[i][0] * _data[i][0];
    }

    return sqrt(norm);
}

void Vect::normalize(void)
{
    double norm = this->norm();
    unsigned int i;

    for (i = 0; i < _rows; i++)
    {
        _data[i][0] /= norm;
    }
}