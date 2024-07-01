#include "EuropeanTrade.h"
#include "Market.h"
#include "Pricer.h"
#include <algorithm>
#include <sstream>

EuropeanOption::EuropeanOption()
    : TreeProduct("EuropeanOption", Date(), "") {}

EuropeanOption::EuropeanOption(OptionType _optType, double _strike, const Date &_expiry)
    : TreeProduct("EuropeanOption", _expiry, ""), optType(_optType), strike(_strike), expiryDate(_expiry) {}

EuropeanOption::EuropeanOption(OptionType optType, double strike, const Date &expiry, const Date &date, const string &underlying, const string& uuid)
    : TreeProduct("EuropeanOption", expiry, date, underlying, uuid), optType(optType), strike(strike), expiryDate(expiry), valueDate(date), underlying(underlying), uuid(uuid) {}

double EuropeanOption::Payoff(double S) const {
    return PAYOFF::VanillaOption(optType, strike, S);
}

std::string EuropeanOption::toString() const {
    std::ostringstream oss;
    oss << "Type: " << OptionTypeToString(optType) << ", Expiry Date: " << expiryDate.toString()
        << ", Underlying: " << underlying << ", UUID: " << uuid;
    return oss.str();
}

const Date& EuropeanOption::GetExpiry() const {
    return expiryDate;
}

double EuropeanOption::getStrike() const {
    return strike;
}

OptionType EuropeanOption::getOptionType() const {
    return optType;
}

double EuropeanOption::ValueAtNode(double S, double t, double continuation) const {
    return continuation;
}

// double EuropeanOption::CalculateVega(const Market &market, const Date &valueDate, Pricer &pricer) const {
//     double originalPV = Pricer::Price(market, this, valueDate);

//     // Perturb the volatility curve
//     VolCurve volCurve = market.getVolCurve(valueDate, underlying);
//     VolCurve upCurve = volCurve;
//     VolCurve downCurve = volCurve;
//     std::vector<Date> tenors = volCurve.getTenors();

//     for (const Date &tenor : tenors) {
//         double currVol = volCurve.getVol(tenor);
//         upCurve.addVol(tenor, currVol + 0.01);
//         downCurve.addVol(tenor, currVol - 0.01);
//     }

//     // Update the market with perturbed curves
//     Market upMarket = market;
//     Market downMarket = market;
//     upMarket.updateVolCurve(underlying, upCurve, valueDate);
//     downMarket.updateVolCurve(underlying, downCurve, valueDate);

//     // Price the option with perturbed curves
//     double pv_up = Pricer::Price(upMarket, this, valueDate);
//     double pv_down = Pricer::Price(downMarket, this, valueDate);

//     // Calculate Vega
//     double vega = (pv_up - pv_down) / 2.0;
//     return vega;
// }

double EuropeanOption::CalculateVega(const Market &market, const Date &valueDate, Pricer *pricer) const {
    double originalPV = pricer->Price(market, this, valueDate);

    // Clone the original market to create perturbed markets
    Market upMarket = market;
    Market downMarket = market;

    // Perturb the volatility curve
    VolCurve originalVolCurve = market.getVolCurve(valueDate, this->underlying);
    VolCurve upVolCurve = originalVolCurve;
    VolCurve downVolCurve = originalVolCurve;

    std::vector<Date> tenors = originalVolCurve.getTenors();
    for (const Date &tenor : tenors) {
        double currVol = originalVolCurve.getVol(tenor);
        upVolCurve.addVol(tenor, currVol + 0.01);  // increase vol by 1%
        downVolCurve.addVol(tenor, currVol - 0.01);  // decrease vol by 1%
    }

    // Update the perturbed markets with new vol curves
    upMarket.updateVolCurve(this->underlying, upVolCurve, valueDate);
    downMarket.updateVolCurve(this->underlying, downVolCurve, valueDate);

    // Price the option with perturbed markets
    double pv_up = pricer->Price(upMarket, this, valueDate);
    double pv_down = pricer->Price(downMarket, this, valueDate);

    // Calculate Vega as the difference in PV divided by the change in volatility
    double vega = (pv_up - pv_down) / 0.02;  // Divided by total change in volatility (0.01 + 0.01)

    return vega;
}

EuroCallSpread::EuroCallSpread(double _k1, double _k2, const Date &_expiry, const Date &_date, const string& _uuid)
    : EuropeanOption(Call, _k1, _expiry, _date, "", _uuid), strike1(_k1), strike2(_k2) {
    assert(_k1 < _k2);
}

double EuroCallSpread::Payoff(double S) const {
    return PAYOFF::CallSpread(strike1, strike2, S);
}
