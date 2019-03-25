#include <utils/matrix.h>
#include <iostream>
#include <iomanip>
#include <math.h>

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

Matrix Matrix::invert()
{
    int i, j;
    int n = _rows;

    Matrix invert(_rows, _columns);

    for (i = 0; i < n; i++)
    {
		for (j = 0; j < n; j++)
        {
            invert.set(i, j, (((i + j) % 2) ? -1.0 : 1.0) * cofactor(i, j).determinate());
        }
    }

    return invert / determinate();
}

Matrix Matrix::minor(int i)
{
	int j, l, h = 0, k = 0;

    if (_rows != _columns)
    {
        return Matrix(0, 0);
    }

    Matrix minor(_rows - 1, _columns - 1);

	for (l = 1; l < _rows; l++)
    {
		for (j = 0; j < _rows; j++)
        {
			if (j != i)
            {
                minor.set(h, k, _data[l][j]);
                k++;
                if (k == (_rows - 1))
                {
                    h++;
                    k = 0;
                }
            }
		}
    }

    return minor;
}

Matrix Matrix::cofactor(int row, int column)
{
	int m, k, i, j;
    int n = _rows;

    Matrix cofactor(_rows - 1, _rows - 1);

    m = 0;
    k = 0;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            if ((i != row) && (j != column))
            {
                cofactor.set(m, k, _data[i][j]);
                if (k < (n - 2))
                {
                    k++;
                }
                else
                {
                    k = 0;
                    m++;
                }
            }
        }
    }

    return cofactor;
}

float Matrix::determinate(void)
{
    int   i;
    float sum = 0.0;

    if (_rows != _columns)
    {
        return 0;
    }

    if (_rows == 1)
    {
        return _data[0][0];
    }
    else if (_rows == 2)
    {
        return (_data[0][0] * _data[1][1] - _data[0][1] * _data[1][0]);
    }
    else
    {
        for (i = 0; i < _rows; i++)
        {
			sum += _data[0][i] * ((i % 2) ? -1.0 : 1.0) * minor(i).determinate();
		}
        return sum;
    }
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

Matrix Matrix::identity(int n)
{
    int i;
    Matrix I(n, n);

    for (i = 0; i < n; i++)
    {
        I.set(i, i, 1.0);
    }

    return I;
}