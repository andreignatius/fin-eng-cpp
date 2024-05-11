#ifndef _BLACK_SCHOLES_PRICER_H
#define _BLACK_SCHOLES_PRICER_H

#include "Market.h"
#include "EuropeanTrade.h"
#include <cmath>

class BlackScholesPricer {
public:
    static double Price(const Market& market, const EuropeanOption& option);
};

#endif
