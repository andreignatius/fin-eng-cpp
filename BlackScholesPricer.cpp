#include "BlackScholesPricer.h"

double BlackScholesPricer::Price(const Market& market, const EuropeanOption& option) {
    double S = market.getSpotPrice(option.getUnderlying());
    double K = option.getStrike();
    double T = (option.GetExpiry() - market.getAsOfDate()) / 365.25;
    double r = market.getRiskFreeRate();
    double sigma = market.getVolatility(option.getUnderlying());

    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);

    if (option.getOptionType() == Call) {
        return S * std::exp(-r * T) * std::normcdf(d1) - K * std::exp(-r * T) * std::normcdf(d2);
    } else {
        return K * std::exp(-r * T) * std::normcdf(-d2) - S * std::exp(-r * T) * std::normcdf(-d1);
    }
}