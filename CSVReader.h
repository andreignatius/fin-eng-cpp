#ifndef CSVREADER_H
#define CSVREADER_H

#include "AmericanTrade.h"
#include "Bond.h"
#include "Date.h"
#include "EuropeanTrade.h"
#include "Market.h"
#include "Swap.h"
#include "Trade.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
class CSVReader {
  // private:
  //   std::string myfilename;
  //   Market theMarket;
  //   vector<Trade *> thePortfolio;

  public:
    std::string myfilename;
    Market theMarket;
    vector<Trade *> thePortfolio;
    // std::string filename;
    // constructors
    CSVReader(const std::string &filename, const Market &marketObj, vector<Trade *> &portfolioVec)
        : myfilename(filename), theMarket(marketObj), thePortfolio(portfolioVec) {
        std::cout << "Start CSVReader constructor" << std::endl;
    };
    // CSVReader(const std::string &filename) : myfilename(filename){};
    CSVReader();
    // Destructor
    ~CSVReader();
    
    // setter getters
    void setFileName(const std::string &filename);
    std::string getFileName() const;

    void setMarketObject(const Market &marketObj);
    Market getMarketObject() const;

    void setPortfolio(vector<Trade *> &portfolioVec);
    vector<Trade *> getPortfolio();

    Market getMarketObject() const;

    // methods
    std::unordered_map<std::string, std::vector<std::string>> parseFile();
    std::vector<std::string> parseRow(const std::string &row,
                                      const char &delim);
    void constructPortfolio();
};
#endif