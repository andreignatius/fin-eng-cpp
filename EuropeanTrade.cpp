#include "EuropeanTrade.h"
#include <algorithm>

EuropeanOption::EuropeanOption()
    : TreeProduct("EuropeanOption", Date(), "") {}

EuropeanOption::EuropeanOption(OptionType _optType, double _strike, const Date &_expiry)
    : TreeProduct("EuropeanOption", _expiry, ""), optType(_optType), strike(_strike), expiryDate(_expiry) {}

EuropeanOption::EuropeanOption(OptionType optType, double strike, const Date &expiry, const string &underlying)
    : TreeProduct("EuropeanOption", expiry, underlying), optType(optType), strike(strike), expiryDate(expiry), underlying(underlying) {}

double EuropeanOption::Payoff(double S) const {
    return PAYOFF::VanillaOption(optType, strike, S);
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

EuroCallSpread::EuroCallSpread(double _k1, double _k2, const Date &_expiry)
    : EuropeanOption(Call, _k1, _expiry, ""), strike1(_k1), strike2(_k2) {
    assert(_k1 < _k2);
}

double EuroCallSpread::Payoff(double S) const {
    return PAYOFF::CallSpread(strike1, strike2, S);
}
