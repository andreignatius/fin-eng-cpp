#ifndef _EUROPEAN_TRADE
#define _EUROPEAN_TRADE

#include <cassert>
#include "TreeProduct.h"
#include "Types.h"
#include "Payoff.h" // Include if PAYOFF namespace functions are defined here

class EuropeanOption : public TreeProduct {
public:
    EuropeanOption();
    EuropeanOption(OptionType _optType, double _strike, const Date &_expiry);
    EuropeanOption(OptionType optType, double strike, const Date &expiry, const string &underlying);

    virtual double Payoff(double S) const override;
    virtual const Date &GetExpiry() const override;
    double getStrike() const;
    OptionType getOptionType() const;
    virtual double ValueAtNode(double S, double t, double continuation) const override;

protected:
    OptionType optType;
    double strike;
    Date expiryDate;
    string underlying;
};

class EuroCallSpread : public EuropeanOption {
public:
    EuroCallSpread(double _k1, double _k2, const Date &_expiry);

    virtual double Payoff(double S) const override;

private:
    double strike1;
    double strike2;
};

#endif // _EUROPEAN_TRADE
