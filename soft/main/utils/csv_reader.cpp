#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <sstream>
#include <utils/csv_reader.h>

std::vector<float> CSVReader::splitRow(std::string line)
{
    std::stringstream     lineStream(line);
    std::string           cell;
    std::vector<float>    vect;

    while(std::getline(lineStream, cell, ','))
    {
        // vect.push_back(std::stof(cell));
    }

    // This checks for a trailing comma with no data after it.
    if (!lineStream && cell.empty())
    {
        // If there was a trailing comma then add an empty element.
        vect.push_back(0.0f);
    }

    return vect;
}

/*
* Parses through csv file line by line and returns the data
* in vector of vector of strings.
*/
std::vector<std::vector<float> > CSVReader::getData()
{
    std::ifstream file(fileName);

    std::vector<std::vector<float>> dataList;

    std::string line = "";
    // Iterate through each line and split the content using delimeter
    while (getline(file, line))
    {
        dataList.push_back(splitRow(line));
    }

    // Close the File
    file.close();
 
    return dataList;
}