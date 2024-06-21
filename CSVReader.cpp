#include "CSVReader.h"

// void CSVReader::setFileName(const std::string &filename) {
//     myfilename = filename;
// }

// std::string CSVReader::getFileName() {
//     return myfilename;
// }

// Market CSVReader::getMarketObject() const { return theMarket; };

void CSVReader::setFileName(const std::string &filename) {
    myfilename = filename;
}

std::string CSVReader::getFileName() const {
    return myfilename;
}

void CSVReader::setMarketObject(const Market &marketObj) {
    theMarket = marketObj;
}

Market CSVReader::getMarketObject() const {
    return theMarket;
}

void CSVReader::setPortfolio(vector<Trade *> &portfolioVec) {
    thePortfolio = portfolioVec;
}

CSVReader::~CSVReader() {
    for (Trade* trade : thePortfolio) {
        delete trade;
    }
    thePortfolio.clear();
}

vector<Trade *> CSVReader::getPortfolio() {
    return thePortfolio;
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

void CSVReader::constructPortfolio() {
    std::ifstream MyReadFile(myfilename);
    std::string lineText;
    std::vector<std::string> headers;
    int lineCount = 0;

    if (!MyReadFile.is_open()) {
        throw std::runtime_error("Could not open file: " + myfilename);
    }

    while (getline(MyReadFile, lineText)) {
        std::vector<std::string> rowData = parseRow(lineText, ';');

        if (lineCount == 0) {
            headers = rowData;
        } else {
            std::map<std::string, std::string> dataMap;
            for (size_t i = 0; i < headers.size(); ++i) {
                dataMap[headers[i]] = i < rowData.size() ? rowData[i] : "";
            }

            if (dataMap["type"] == "bond") {
                auto bond = new Bond(
                    Date::FromString(dataMap["start"]),
                    Date::FromString(dataMap["end"]),
                    std::stod(dataMap["notional"]),
                    std::stod(dataMap["strike"]), // Assuming 'strike' here represents coupon rate for bonds
                    dataMap["underlying"],
                    dataMap["Id"]);
                thePortfolio.push_back(bond);
            } else if (dataMap["type"] == "swap") {
                auto swap = new Swap(
                    Date::FromString(dataMap["start"]),
                    Date::FromString(dataMap["end"]),
                    std::stod(dataMap["notional"]),
                    std::stod(dataMap["strike"]), // Assuming 'strike' is used for the fixed rate in swaps
                    std::stod(dataMap["freq"]),
                    dataMap["fixed_for_float"] == "true",
                    theMarket, // Use the member Market object
                    dataMap["curveName"],
                    dataMap["Id"]);
                thePortfolio.push_back(swap);
            } else if (dataMap["type"].find("opt") != std::string::npos) {
                // Differentiate between European and American options
                Option* option;
                if (dataMap["type"].substr(0, 3) == "eur") {
                    option = new EuropeanOption(
                        dataMap["underlying"],
                        Date::FromString(dataMap["end"]),
                        std::stod(dataMap["strike"]),
                        dataMap["opt"],
                        std::stoi(dataMap["notional"]),
                        dataMap["Id"]);
                } else {
                    option = new AmericanOption(
                        dataMap["underlying"],
                        Date::FromString(dataMap["end"]),
                        std::stod(dataMap["strike"]),
                        dataMap["opt"],
                        std::stoi(dataMap["notional"]),
                        dataMap["Id"]);
                }
                thePortfolio.push_back(option);
            }
        }
        ++lineCount;
    }
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