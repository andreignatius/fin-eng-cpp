#include "BlackScholesPricer.h"
#include "Constants.h"
#include <random>

double BlackScholesPricer::normcdf(double value) {
    return 0.5 * std::erfc(-value * std::sqrt(0.5));
}

double BlackScholesPricer::Price(const Market& market, const EuropeanOption& option) {
    double S = market.getSpotPrice(option.getUnderlying());
    double K = option.getStrike();
    std::cout << "Expiry: " << option.GetExpiry() << ", Market as of: " << market.asOf << std::endl;
    std::cout << "Diff in days: " << option.GetExpiry().differenceInDays(market.asOf) << std::endl;
    double T = option.GetExpiry().differenceInDays(market.asOf) / Constants::NUM_DAYS_IN_YEAR;
    double r = market.getRiskFreeRate();
    double sigma = market.getVolCurve("EuropeanOption").getVol(option.GetExpiry());
    std::cout << "BS pricer parameters "<<std::endl;
    std::cout << "S: " << S << " K: " << K << " T: " << T << " r: " << r << std::endl;

    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);

    if (option.getOptionType() == Call) {
        return S * normcdf(d1) - K * std::exp(-r * T) * normcdf(d2);
    } else {
        return K * std::exp(-r * T) * normcdf(-d2) - S * normcdf(-d1);
    }
}
