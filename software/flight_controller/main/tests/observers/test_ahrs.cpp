#include <iostream>
#include <utils/csv_reader.h>
#include <utils/madgwick.h>

int main()
{
    // Creating an object of CSVWriter
    CSVReader reader("../data/time-gyr-acc-mag.csv");

    Madgwick madgwick(0.01, 0.00390625);

    std::cout << madgwick.roll() << " " << madgwick.pitch() << " " << madgwick.yaw() << std::endl;
 
    // Get the data from CSV File
    std::vector<std::vector<float> > dataList = reader.getData();
 
    // Print the content of row by row on screen
    for(std::vector<float> vec : dataList)
    {
        madgwick.update(vec[1] * 3.14159265359/180.0, vec[2] * 3.14159265359/180.0, vec[3] * 3.14159265359/180.0, vec[4], vec[5], vec[6], vec[7], vec[8], vec[9]);
        std::cout << madgwick.roll() << " " << madgwick.pitch() << " " << madgwick.yaw() << std::endl;
    }

    return 0; 
}
