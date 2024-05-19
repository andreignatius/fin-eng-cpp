#ifndef JSONREADER_H
#define JSONREADER_H
#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class JSONReader {
    /*
        The JSONReader function expects the following at the very least
        {
            "name":"dataname",
            "records":[
                {"col1":"key1","col2":"value1"},
                {"col1":"key2","col2":"value2"},
            ]
        }

        or you can refer to example in /data/sofrdummycurve.json
    */
  private:
    std::string myfilename;

  public:
    // constructors
    JSONReader(const std::string &filename) : myfilename(filename){};
    JSONReader();

    // methods
    std::unordered_map<std::string, std::vector<std::string>> parseFile();
    std::vector<std::string> parseRow(const std::string &row,
                                      const char &delim);
    void setFileName(const std::string &filename);
    std::string getFileName(const std::string &filename);

    // inplace leading trim
    inline void ltrim(std::string &s) {
        s.erase(s.begin(),
                std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }));
    }
    // inplace trailing trim
    inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             [](unsigned char ch) { return !std::isspace(ch); })
                    .base(),
                s.end());
    }
    // inplace trim
    inline void trim(std::string &s) {
        rtrim(s);
        ltrim(s);
    }
};
#endif