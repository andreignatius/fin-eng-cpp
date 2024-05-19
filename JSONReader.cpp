#include "JSONReader.h"

void JSONReader::setFileName(const std::string &filename) {
    myfilename = filename;
}

std::string JSONReader::getFileName(const std::string &filename) {
    return myfilename;
}

/*
    this is the main file parsing function
*/
std::unordered_map<std::string, std::vector<std::string>>
JSONReader::parseFile() {
    std::string lineText;
    std::ifstream MyReadFile(myfilename);
    std::unordered_map<std::string, std::vector<std::string>> myMap;
    std::vector<std::string> keyvector;
    std::vector<std::string> rowdatavector;
    // trust me this is the regex to extract the records
    std::regex pattern(R"(\{\"(.*)\":\"([^\"]+)\",\"(.*)\":\"([^\"]+)\"\})");
    std::smatch match;

    bool startRecord = false;
    int linecount = 0;

    while (getline(MyReadFile, lineText)) {

        trim(lineText);
        // get name
        if (lineText.find("name") != std::string::npos) {
            std::string dirtyName = parseRow(lineText, ':')[1];
            // remove the annoying comma in the end
            std::cout << dirtyName.erase(dirtyName.size() - 1) << std::endl;
        }
        // get the column first
        if (lineText.find("record") != std::string::npos) {
            startRecord = true;
        }

        // get keys and record
        if (std::regex_search(lineText, match, pattern)) {
            if (match.size() > 1) {
                if (startRecord) {
                    std::cout << "key found " << match.str(1) << " "
                              << match.str(3) << std::endl;
                    keyvector.push_back(match.str(1));
                    keyvector.push_back(match.str(3));
                    for (std::string element : keyvector) {
                        myMap[element] = std::vector<std::string>{};
                    }
                    startRecord = false;
                }

                rowdatavector = {match.str(2), match.str(4)};
                int index = 0;
                for (std::string element : keyvector) {
                    std::cout << rowdatavector[index] << std::endl;
                    myMap[element].push_back(rowdatavector[index]);
                    index = index + 1;
                }
            }
        }
    }
    MyReadFile.close();
    return myMap;
}

std::vector<std::string> JSONReader::parseRow(const std::string &row,
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
    JSONReader myJSONReader = JSONReader("./data/sofrdummycurve.json");
    std::unordered_map<std::string, std::vector<std::string>> myMap =
        myJSONReader.parseFile();

    // get map keys
    std::cout << "Map keys:" << std::endl;
    for (const auto &pair : myMap) {
        std::cout << pair.first << std::endl;
    }
    std::cout << "get tenors:" << std::endl;
    // trial sample data
    for (const auto &pair : myMap["tenor"]) {
        std::cout << pair << std::endl;
    }
    std::cout << "get rates:" << std::endl;
    // trial another data
    for (const auto &pair : myMap["rate"]) {
        std::cout << pair << std::endl;
    }

    return 0;
}
*/