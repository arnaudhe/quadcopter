#include <utils/vector.h>
#include <math.h>
#include <esp_attr.h>

IRAM_ATTR Vect::Vect(unsigned int dim, float initial):
    Matrix(dim, 1, initial)
{
}

IRAM_ATTR Vect::Vect(Matrix mat):
    Matrix(mat.rows(), 1)
{
    set_data(mat.data());
}

void IRAM_ATTR Vect::set(unsigned int index, float value)
{
    _data[index][0] = value;
}

float IRAM_ATTR Vect::operator()(const unsigned &index)
{
    return _data[index][0];
}

float IRAM_ATTR Vect::norm(void)
{
    unsigned int i;
    float norm = 0.0f;

    for (i = 0; i < _rows; i++)
    {
        norm += _data[i][0] * _data[i][0];
    }

    return sqrt(norm);
}

void IRAM_ATTR Vect::normalize(void)
{
    float norm = this->norm();
    unsigned int i;

    for (i = 0; i < _rows; i++)
    {
        _data[i][0] /= norm;
    }
}