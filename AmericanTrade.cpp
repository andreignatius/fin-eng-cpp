#include "AmericanTrade.h"
#include "Market.h"
#include "Pricer.h"
#include <algorithm>
#include <sstream>

AmericanOption::AmericanOption(OptionType optType, double strike, const Date& expiry, const Date& date, const string& underlying, const string& uuid)
    : TreeProduct("AmericanOption", expiry, date, underlying, uuid), optType(optType), strike(strike), expiryDate(expiry), valueDate(date), underlying(underlying), uuid(uuid) {
    }

double AmericanOption::Payoff(double S) const {
    return PAYOFF::VanillaOption(optType, strike, S);
}

std::string AmericanOption::toString() const {
    std::ostringstream oss;
    oss << "Type: " << OptionTypeToString(optType) << ", Expiry Date: " << expiryDate.toString()
        << ", Underlying: " << underlying << ", UUID: " << uuid;
    return oss.str();
}

const Date& AmericanOption::GetExpiry() const {
    return expiryDate;
}

double AmericanOption::getStrike() const { 
	return strike; 
}

OptionType AmericanOption::getOptionType() const { 
	return optType; 
}

double AmericanOption::AmericanOption::ValueAtNode(double S, double t, double continuation) const {
    return std::max(Payoff(S), continuation);
}

// double AmericanOption::CalculateVega(const Market &market, const Date &valueDate, Pricer &pricer) const {
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
//     double pv_up = pricer->Price(upMarket, this, valueDate);
//     double pv_down = pricer->Price(downMarket, this, valueDate);

//     // Calculate Vega
//     double vega = (pv_up - pv_down) / 2.0;
//     return vega;
// }

double AmericanOption::CalculateVega(const Market &market, const Date &valueDate, Pricer *pricer) const {
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


AmerCallSpread::AmerCallSpread(double k1, double k2, const Date& expiry, const Date& date, const string& uuid)
    : TreeProduct("AmerCallSpread", expiry, date, "", uuid), strike1(k1), strike2(k2), expiryDate(expiry), valueDate(date) {
    assert(k1 < k2); // Assert condition to ensure valid strikes
}

double AmerCallSpread::Payoff(double S) const {
    return PAYOFF::CallSpread(strike1, strike2, S);
}

const Date& AmerCallSpread::GetExpiry() const {
    return expiryDate;
}