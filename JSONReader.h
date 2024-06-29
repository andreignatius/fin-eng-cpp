#ifndef JSONREADER_H
#define JSONREADER_H
#include "AmericanTrade.h"
#include "Bond.h"
#include "Date.h"
#include "EuropeanTrade.h"
#include "Market.h"
#include "Swap.h"
#include "Trade.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory> // Include for std::unique_ptr
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class JSONReader {
  public:
    std::string theFilename;
    Market theMarket;
    std::vector<std::unique_ptr<Trade>> thePortfolio; // Use smart pointers

    // constructors

    JSONReader(const std::string &filename, const Market& marketObj, std::vector<std::unique_ptr<Trade>>& portfolioVec)
        : theFilename(filename), theMarket(marketObj), thePortfolio(std::move(portfolioVec)) {
        std::cout << "Start JSONReader constructor with reference" << std::endl;
    };
    
    // Destructor
    ~JSONReader();

    // setter getters
    void setFileName(const std::string &filename);
    std::string getFileName() const;

    void setMarketObject(const Market &marketObj);
    Market getMarketObject() const;

    // no const because we will be modifying god willing
    void setPortfolio(vector<std::unique_ptr<Trade>> &portfolioVec);
    vector<std::unique_ptr<Trade>>& getPortfolio();

    // methods
    std::vector<std::string> parseRow(const std::string &row,
                                      const char &delim);
    void constructPortfolio();

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
#endif // jsonreader