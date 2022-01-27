#include <iostream>
#include <math.h>
#include <utils/filter.h>

using namespace std;

#define PERIOD        0.002
#define SIGNAL_FREQ   100.0

#define M_PIf 3.14159265359f

int main()
{
    PT2Filter filter_20(PERIOD, 20.0);
    PT2Filter filter_50(PERIOD, 50.0);
    PT2Filter filter_80(PERIOD, 80.0);

    filter_20.init();
    filter_50.init();
    filter_80.init();

    cout << "input" << ";" << "20_hz" << ";" << "50_hz" << ";" << "80_hz" << endl;

    for (int i = 0; i < 5 * (1.0 / PERIOD); i++)
    {
        float time = i * PERIOD;
        float input = sinf(i * PERIOD * 2.0 * M_PIf * SIGNAL_FREQ);
        cout << time << ";" << input << ";" << filter_20.apply(input) << ";" << filter_50.apply(input) << ";" << filter_80.apply(input) << endl;
    }

    return 0;
}
