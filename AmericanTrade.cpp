#include "AmericanTrade.h"
#include "Market.h"
#include "Pricer.h"
#include <algorithm>
#include <sstream>

AmericanOption::AmericanOption(OptionType optType, double strike,
                               const Date &expiry, const Date &date,
                               const string &underlying, const string &uuid)
    : TreeProduct("AmericanOption", expiry, date, underlying, uuid),
      optType(optType), strike(strike), expiryDate(expiry), valueDate(date),
      underlying(underlying), uuid(uuid) {}

double AmericanOption::Payoff(double S) const {
    return PAYOFF::VanillaOption(optType, strike, S);
}

std::string AmericanOption::toString() const {
    std::ostringstream oss;
    oss << "Type: " << OptionTypeToString(optType)
        << ", Expiry Date: " << expiryDate.toString()
        << ", Underlying: " << underlying << ", UUID: " << uuid;
    return oss.str();
}

const Date &AmericanOption::GetExpiry() const { return expiryDate; }

double AmericanOption::getStrike() const { return strike; }

OptionType AmericanOption::getOptionType() const { return optType; }

double AmericanOption::AmericanOption::ValueAtNode(double S, double t,
                                                   double continuation) const {
    return std::max(Payoff(S), continuation);
}

std::vector<double> AmericanOption::CalculateDV01(const Market &market,
                                                  const Date &valueDate,
                                                  Pricer *pricer) const {
    std::vector<double> dv01_output;
    std::cout << "***AmericanOption CalculateDV01 START" << std::endl;
    // double originalPV = pricer->Price(market, this, valueDate);
    RateCurve theCurve = market.getCurve(valueDate, "usd-sofr");
    RateCurve upCurve = theCurve;
    RateCurve downCurve = theCurve;
    std::vector<Date> tenors = theCurve.getTenors();
    for (auto it = tenors.begin(); it != tenors.end(); ++it) {
        double currRate = theCurve.getRate(*it);
        upCurve.addRate(*it,
                        currRate + Constants::YIELD_CURVE_SHOCK_SIZE_SINGLE_BP);
        downCurve.addRate(*it, currRate -
                                   Constants::YIELD_CURVE_SHOCK_SIZE_SINGLE_BP);
        // Clone the original market to create perturbed markets
        Market upMarket = market;
        Market downMarket = market;
        // assume usd-sofr curve
        upMarket.updateRateCurve("usd-sofr", upCurve, valueDate);
        downMarket.updateRateCurve("usd-sofr", downCurve, valueDate);
        // Price the option with perturbed markets
        double pv_up = pricer->Price(upMarket, this, valueDate);
        double pv_down = pricer->Price(downMarket, this, valueDate);

        double dv01 = (pv_up - pv_down) / 2.0;
        dv01_output.push_back(dv01);
    }
    std::cout << "***AmericanOption CalculateDV01 END" << std::endl;
    return dv01_output;
}

std::vector<double> AmericanOption::CalculateVega(const Market &market,
                                                  const Date &valueDate,
                                                  Pricer *pricer) const {
    std::vector<double> vega_output;
    std::cout << "***AmericanOption CalculateVega START" << std::endl;
    // Perturb the volatility curve
    std::string underlying = this->underlying;
    VolCurve originalVolCurve = market.getVolCurve(valueDate, underlying);
    VolCurve upVolCurve = originalVolCurve;
    VolCurve downVolCurve = originalVolCurve;
    std::vector<Date> tenors = originalVolCurve.getTenors();
    for (const Date &tenor : tenors) {
        double currVol = originalVolCurve.getVol(tenor);
        upVolCurve.addVol(
            tenor,
            currVol +
                Constants::YIELD_CURVE_SHOCK_SIZE_SINGLE_PERCENT); // increase
                                                                   // vol by 1%
        downVolCurve.addVol(
            tenor,
            currVol -
                Constants::
                    YIELD_CURVE_SHOCK_SIZE_SINGLE_PERCENT); // decrease
                                                            // vol by 1%
                                                            // Clone the
                                                            // original market
                                                            // to create
                                                            // perturbed markets
        Market upMarket = market;
        Market downMarket = market;
        // assume usd-sofr curve
        upMarket.updateVolCurve(underlying, upVolCurve, valueDate);
        downMarket.updateVolCurve(underlying, downVolCurve, valueDate);

        // Price the option with perturbed markets
        double pv_up = pricer->Price(upMarket, this, valueDate);
        double pv_down = pricer->Price(downMarket, this, valueDate);

        // Calculate Vega as the difference in PV divided by the change in
        // volatility
        double vega = (pv_up - pv_down) /
                      (2.0 * Constants::YIELD_CURVE_SHOCK_SIZE_SINGLE_PERCENT);
        vega_output.push_back(vega);
    }

    std::cout << "***AmericanOption CalculateVega END" << std::endl;
    return vega_output;
}

AmerCallSpread::AmerCallSpread(double k1, double k2, const Date &expiry,
                               const Date &date, const string &uuid)
    : TreeProduct("AmerCallSpread", expiry, date, "", uuid), strike1(k1),
      strike2(k2), expiryDate(expiry), valueDate(date) {
    assert(k1 < k2); // Assert condition to ensure valid strikes
}

double AmerCallSpread::Payoff(double S) const {
    return PAYOFF::CallSpread(strike1, strike2, S);
}

const Date &AmerCallSpread::GetExpiry() const { return expiryDate; }