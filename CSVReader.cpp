#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> parseRow(const std::string &row, const char &delim);

int main()
{
    std::string filename = "sofrdummycurve.csv";
    std::string lineText;
    std::ifstream MyReadFile(filename);
    std::vector<std::string> outputvector;
    while (getline(MyReadFile, lineText))
    {
        std::cout << "line to parse : " << lineText << std::endl;
        outputvector = parseRow(lineText, ',');
        for (auto i : outputvector)
        {
            std::cout << i << std::endl;
        }
        std::cout << std::endl;
    }
    MyReadFile.close();
    return 0;
}

std::vector<std::string> parseRow(const std::string &row, const char &delim)
{
    std::vector<std::string> result;
    std::stringstream ss(row);
    std::string item;

    while (getline(ss, item, delim))
    {
        result.push_back(item);
    }

    return result;
}