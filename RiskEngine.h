#ifndef RISKENGINE
#define RISKENGINE
#include "AmericanTrade.h"
#include "Bond.h"
#include "EuropeanTrade.h"
#include "Swap.h"
#include "Trade.h"
#include "Pricer.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <vector>

class RiskEngine {
  public:
    RiskEngine(const Market &_market) : theMarket(_market) {
        theMarket.Print();
    };
    // virtual double Price(const Market& mkt, std::unique_ptr<Trade>& trade);
    void computeRisk(string riskType, Trade *trade, Date valueDate, Pricer* pricer,
    				std::vector<double>& dv01_output, std::vector<double>& vega_output);

  private:
    Market theMarket;
};
#endif
