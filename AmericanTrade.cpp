#include "AmericanTrade.h"
#include <algorithm>

AmericanOption::AmericanOption(OptionType optType, double strike, const Date& expiry, const string& underlying)
    : TreeProduct("AmericanOption", expiry, underlying), optType(optType), strike(strike), expiryDate(expiry) {
    }

double AmericanOption::Payoff(double S) const {
    return PAYOFF::VanillaOption(optType, strike, S);
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

AmerCallSpread::AmerCallSpread(double k1, double k2, const Date& expiry)
    : TreeProduct("AmerCallSpread", expiry, ""), strike1(k1), strike2(k2), expiryDate(expiry) {
    assert(k1 < k2); // Assert condition to ensure valid strikes
}

double AmerCallSpread::Payoff(double S) const {
    return PAYOFF::CallSpread(strike1, strike2, S);
}

const Date& AmerCallSpread::GetExpiry() const {
    return expiryDate;
}