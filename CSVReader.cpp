#include "CSVReader.h"

void CSVReader::setFileName(const std::string &filename) {
    myfilename = filename;
}
std::string CSVReader::getFileName(const std::string &filename) {
    return myfilename;
}
std::unordered_map<std::string, std::vector<std::string>>
CSVReader::parseFile() {
    std::string lineText;
    std::ifstream MyReadFile(myfilename);
    std::unordered_map<std::string, std::vector<std::string>> myMap;
    std::vector<std::string> keyvector;
    std::vector<std::string> rowdatavector;

    int linecount = 0;

    while (getline(MyReadFile, lineText)) {
        // this is to get headers
        if (linecount == 0) {
            keyvector = parseRow(lineText, ',');
            for (std::string element : keyvector) {
                myMap[element] = std::vector<std::string>{};
            }
        } else {
            rowdatavector = parseRow(lineText, ',');
            int index = 0;
            for (std::string element : keyvector) {
                myMap[element].push_back(rowdatavector[index]);
                index = index + 1;
            }
        }
        linecount++;
    }
    MyReadFile.close();
    return myMap;
}
std::vector<std::string> CSVReader::parseRow(const std::string &row,
                                             const char &delim) {
    std::vector<std::string> result;
    std::stringstream ss(row);
    std::string item;
    while (getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

/*
int main() {
    CSVReader myCSVReader = CSVReader("sofrdummycurve.csv");
    std::unordered_map<std::string, std::vector<std::string>> myMap =
        myCSVReader.parseFile();

    // get map keys
    std::cout << "Map keys:" << std::endl;
    for (const auto &pair : myMap) {
        std::cout << pair.first << std::endl;
    }

    // trial sample data
    for (const auto &pair : myMap["tenor"]) {
        std::cout << pair << std::endl;
    }

    // trial another data
    for (const auto &pair : myMap["rate"]) {
        std::cout << pair << std::endl;
    }

    return 0;
}
*/