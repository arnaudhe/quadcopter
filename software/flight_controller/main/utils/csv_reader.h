#pragma once

#include <string>
#include <vector>

/*
 * A class to read data from a csv file.
 */
class CSVReader
{
    std::string fileName;
    std::string delimeter;

    std::vector<float> splitRow(std::string line);
 
public:
    CSVReader(std::string filename, std::string delm = ",") :
            fileName(filename), delimeter(delm)
    { }

    // Function to fetch data from a CSV File
    std::vector<std::vector<float> > getData();
};