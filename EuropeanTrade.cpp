#include "EuropeanTrade.h"
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

EuroCallSpread::EuroCallSpread(double _k1, double _k2, const Date &_expiry, const Date &_date, const string& _uuid)
    : EuropeanOption(Call, _k1, _expiry, _date, "", _uuid), strike1(_k1), strike2(_k2) {
    assert(_k1 < _k2);
}

double EuroCallSpread::Payoff(double S) const {
    return PAYOFF::CallSpread(strike1, strike2, S);
}
