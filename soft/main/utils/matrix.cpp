#include <utils/matrix.h>
#include <iostream>
#include <iomanip>

Matrix::Matrix(unsigned int rows, unsigned int columns, double initial)
{
    _rows = rows;
    _columns = columns;

    _data.resize(_rows);

    for (unsigned i = 0; i < _data.size(); i++)
    {
        _data[i].resize(_columns, initial);
    }
}

unsigned int Matrix::rows(void)
{
    return _rows;
}

unsigned int Matrix::columns(void)
{
    return _columns;
}

vector<vector<float>> Matrix::data()
{
    return _data;
}

void Matrix::set_data(vector<vector<float>> data)
{
    _data = data;
}

void Matrix::set(unsigned row, unsigned column, float value)
{
    _data[row][column] = value;
}

float Matrix::operator()(const unsigned &row, const unsigned & column)
{
    return _data[row][column];
}

// Addition of Two Matrices
Matrix Matrix::operator+(Matrix B)
{
    Matrix sum(_rows, _columns, 0.0);
    unsigned i,j;

    for (i = 0; i < _rows; i++)
    {
        for (j = 0; j < _columns; j++)
        {
            sum.set(i, j, this->_data[i][j] + B(i, j));
        }
    }
    return sum;
}

// Subtraction of Two Matrices
Matrix Matrix::operator-(Matrix B)
{
    Matrix diff(_rows, _columns, 0.0);
    unsigned i,j;

    for (i = 0; i < _rows; i++)
    {
        for (j = 0; j < _columns; j++)
        {
            diff.set(i, j, this->_data[i][j] - B(i, j));
        }
    }
    
    return diff;
}

// Multiplication of Two Matrices
Matrix Matrix::operator*(Matrix B)
{
    Matrix multip(_rows, B.columns(), 0.0);

    if(_columns == B.rows())
    {
        unsigned i,j,k;
        double temp = 0.0;
        for (i = 0; i < _rows; i++)
        {
            for (j = 0; j < B.columns(); j++)
            {
                temp = 0.0;
                for (k = 0; k < _columns; k++)
                {
                    temp += _data[i][k] * B(k, j);
                }
                multip.set(i, j, temp);
                //cout << multip(i,j) << " ";
            }
            //cout << endl;
        }
        return multip;
    }
    else
    {
        return Matrix(0, 0);
    }
}


// Scalar Addition
Matrix Matrix::operator+(float scalar)
{
    Matrix result(_rows, _columns, 0.0);
    unsigned i,j;
    for (i = 0; i < _rows; i++)
    {
        for (j = 0; j < _columns; j++)
        {
            result.set(i, j, this->_data[i][j] + scalar);
        }
    }
    return result;
}

// Scalar Subraction
Matrix Matrix::operator-(float scalar)
{
    Matrix result(_rows, _columns, 0.0);
    unsigned i,j;
    for (i = 0; i < _rows; i++)
    {
        for (j = 0; j < _columns; j++)
        {
            result.set(i, j, this->_data[i][j] - scalar);
        }
    }
    return result;
}

// Scalar Multiplication
Matrix Matrix::operator*(float scalar)
{
    Matrix result(_rows, _columns, 0.0);
    unsigned i,j;
    for (i = 0; i < _rows; i++)
    {
        for (j = 0; j < _columns; j++)
        {
            result.set(i, j, this->_data[i][j] * scalar);
        }
    }
    return result;
}

// Scalar Division
Matrix Matrix::operator/(float scalar)
{
    Matrix result(_rows, _columns, 0.0);
    unsigned i,j;
    for (i = 0; i < _rows; i++)
    {
        for (j = 0; j < _columns; j++)
        {
            result.set(i, j, this->_data[i][j] / scalar);
        }
    }
    return result;
}

Matrix Matrix::transpose()
{
    Matrix transp(_columns, _rows,0.0);
    for (unsigned i = 0; i < _columns; i++)
    {
        for (unsigned j = 0; j < _rows; j++) {
            transp.set(i, j, this->_data[j][i]);
        }
    }
    return transp;
}

void Matrix::display(string name)
{
    int i, j;

    std::cout.precision(5);

    std::cout << name << " (matrix " << _rows << "x" << _columns << ")" << std::endl;
    for (i = 0; i < _rows; i++)
    {
        for (j = 0; j < _columns; j++)
        {
            std::cout << fixed << setprecision(3) << _data[i][j] << " ";
        }
        std::cout << std::endl;
    }
}