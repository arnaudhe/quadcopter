#include <iostream>
#include <math.h>
#include <utils/filter.h>

using namespace std;

#define PERIOD        0.002
#define SIGNAL_FREQ   20.0

#define M_PIf 3.14159265359f

int main()
{
    float input = 0.0;
    SlewFilter filter(PERIOD, 50.0);

    filter.init();

    cout << "input" << ";" << "filtered" << endl;

    for (int i = 0; i < 10; i++)
    {
        cout << input << ";" << filter.apply(input) << endl;
    }

    input = 1.0;

    for (int i = 0; i < 50; i++)
    {
        cout << input << ";" << filter.apply(input) << endl;
    }

    input = 0.0;

    for (int i = 0; i < 50; i++)
    {
        cout << input << ";" << filter.apply(input) << endl;
    }

    for (int i = 0; i < (1.0 / PERIOD); i++)
    {
        input = sinf(i * PERIOD * 2.0 * M_PIf * SIGNAL_FREQ);
        cout << input << ";" << filter.apply(input) << endl;
    }

    return 0;
}
