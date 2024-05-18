#ifndef _AMERICAN_TRADE
#define _AMERICAN_TRADE

#include <cassert>
#include "TreeProduct.h"
#include "Types.h"
#include "Payoff.h" // Assuming necessary for PAYOFF namespace functions

class AmericanOption : public TreeProduct {
public:
    AmericanOption(OptionType optType, double strike, const Date& expiry, const string& underlying);
    virtual double Payoff(double S) const override;
    virtual const Date& GetExpiry() const override;
    double getStrike() const;
    OptionType getOptionType() const;
    virtual double ValueAtNode(double S, double t, double continuation) const override;

private:
    OptionType optType;
    double strike;
    Date expiryDate;
    string underlying;
};

class AmerCallSpread : public TreeProduct {
public:
    AmerCallSpread(double k1, double k2, const Date& expiry);
    virtual double Payoff(double S) const override;
    virtual const Date& GetExpiry() const override;

private:
    double strike1;
    double strike2;
    Date expiryDate;
};

#endif
