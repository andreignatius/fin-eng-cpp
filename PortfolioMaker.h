#ifndef PORTFOLIOMAKER
#define PORTFOLIOMAKER
#include "AmericanTrade.h"
#include "Bond.h"
#include "Date.h"
#include "EuropeanTrade.h"
#include "Market.h"
#include "Swap.h"
#include "Trade.h"
#include <fstream>
#include <iostream>
#include <memory> // Include for std::unique_ptr
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
class PortfolioMaker {
  public:
    static std::vector<std::unique_ptr<Trade>> constructPortfolio(
        const Date &valueDate,
        const std::unordered_map<std::string, std::vector<std::string>>
            &portfolioMap,
        const Market &theMarket);
    static std::vector<std::string> parseDelimiter(const std::string &row,
                                                   const char &delim);
};

#endif