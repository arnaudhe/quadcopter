#include <iostream>
#include <utils/csv_reader.h>
#include <utils/height_observer.h>

int main()
{
    // Creating an object of CSVWriter
    CSVReader reader("../data/time-acc-baro.csv");

    HeightObserver observer(0.01);
 
    // Get the data from CSV File
    std::vector<std::vector<float> > dataList = reader.getData();
 
    // Print the content of row by row on screen
    for(std::vector<float> vec : dataList)
    {
        observer.update(vec[1], vec[2]);
        std::cout << observer.height() << " " << observer.vertical_speed() << std::endl;
    }

    return 0; 
}
