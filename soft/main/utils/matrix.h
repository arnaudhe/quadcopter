#pragma once

#include <vector>
#include <string>

using namespace std;

class Matrix
{
  protected:

    unsigned int            _rows;
    unsigned int            _columns;
    vector<vector<float>>   _data;

  public:

    Matrix(unsigned int, unsigned int, double initial = 0.0);

    vector<vector<float>> data();
    void set_data(vector<vector<float>>);
    void set(unsigned int, unsigned int, float);

    unsigned int rows(void);
    unsigned int columns(void);

    Matrix row(unsigned int row);
    Matrix columns(unsigned int column);
    Matrix transpose(void);
    Matrix invert(void);
    Matrix minor(int i);
    Matrix cofactor(int row, int column);
  
    float determinate(void);

    void display(string name);

    float  operator()(const unsigned &, const unsigned &);
    Matrix operator+(Matrix);
    Matrix operator-(Matrix);
    Matrix operator*(Matrix);
    Matrix operator%(Matrix);
    Matrix operator+(float);
    Matrix operator-(float);
    Matrix operator*(float);
    Matrix operator/(float);

    static Matrix identity(int n);

};