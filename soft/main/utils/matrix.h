#pragma once

#include <vector>
#include <string>

using namespace std;

class Matrix
{
  protected:

    unsigned int            _rows;
    unsigned int            _columns;
    vector<vector<double> >  _data;

  public:

    Matrix(unsigned int, unsigned int, double initial = 0.0);

    vector<vector<double> > data();
    void set_data(vector<vector<double> >);
    void set(unsigned int, unsigned int, double);

    unsigned int rows(void);
    unsigned int columns(void);

    Matrix row(unsigned int row);
    Matrix columns(unsigned int column);
    Matrix transpose(void);
    Matrix invert(void);
    Matrix minor(int i);
    Matrix cofactor(int row, int column);
  
    double determinate(void);

    void display(string name);

    double operator()(const unsigned &, const unsigned &);
    Matrix operator+(Matrix);
    Matrix operator-(Matrix);
    Matrix operator*(Matrix);
    Matrix operator%(Matrix);
    Matrix operator+(double);
    Matrix operator-(double);
    Matrix operator*(double);
    Matrix operator/(double);

    static Matrix identity(int n);

};