#include "EuropeanTrade.h"
#include "Constants.h"
#include "Market.h"
#include "Pricer.h"
#include <algorithm>
#include <sstream>

EuropeanOption::EuropeanOption() : TreeProduct("EuropeanOption", Date(), "") {}

EuropeanOption::EuropeanOption(OptionType _optType, double _strike,
                               const Date &_expiry)
    : TreeProduct("EuropeanOption", _expiry, ""), optType(_optType),
      strike(_strike), expiryDate(_expiry) {}

EuropeanOption::EuropeanOption(OptionType optType, double strike,
                               const Date &expiry, const Date &date,
                               const string &underlying, const string &uuid)
    : TreeProduct("EuropeanOption", expiry, date, underlying, uuid),
      optType(optType), strike(strike), expiryDate(expiry), valueDate(date),
      underlying(underlying), uuid(uuid) {}

double EuropeanOption::Payoff(double S) const {
    return PAYOFF::VanillaOption(optType, strike, S);
}

std::string EuropeanOption::toString() const {
    std::ostringstream oss;
    oss << "Type: " << OptionTypeToString(optType)
        << ", Expiry Date: " << expiryDate.toString()
        << ", Underlying: " << underlying << ", UUID: " << uuid;
    return oss.str();
}

const Date &EuropeanOption::GetExpiry() const { return expiryDate; }

double EuropeanOption::getStrike() const { return strike; }

OptionType EuropeanOption::getOptionType() const { return optType; }

double EuropeanOption::ValueAtNode(double S, double t,
                                   double continuation) const {
    return continuation;
}

std::vector<double> EuropeanOption::CalculateDV01(const Market &market,
                                                  const Date &valueDate,
                                                  Pricer *pricer) const {
    std::vector<double> dv01_output;
    std::cout << "***EuropeanOption CalculateDV01 START" << std::endl;
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
    std::cout << "***EuropeanOption CalculateDV01 END" << std::endl;
    return dv01_output;
}

std::vector<double> EuropeanOption::CalculateVega(const Market &market,
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

    std::cout << "***EuropeanOption CalculateVega END" << std::endl;
    return vega_output;
}

EuroCallSpread::EuroCallSpread(double _k1, double _k2, const Date &_expiry,
                               const Date &_date, const string &_uuid)
    : EuropeanOption(Call, _k1, _expiry, _date, "", _uuid), strike1(_k1),
      strike2(_k2) {
    assert(_k1 < _k2);
}

double EuroCallSpread::Payoff(double S) const {
    return PAYOFF::CallSpread(strike1, strike2, S);
}
