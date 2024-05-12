#ifndef CSVREADER_H
#define CSVREADER_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
class CSVReader {
  private:
    std::string myfilename;

  public:
    std::string filename;
    CSVReader(const std::string &filename) : myfilename(filename){};
    CSVReader();
    std::unordered_map<std::string, std::vector<std::string>> parseFile();
    std::vector<std::string> parseRow(const std::string &row,
                                      const char &delim);
    void setFileName(const std::string &filename);
    std::string getFileName(const std::string &filename);
};
#endif