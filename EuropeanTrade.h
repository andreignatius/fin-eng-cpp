#ifndef _EUROPEAN_TRADE
#define _EUROPEAN_TRADE

#include <cassert> 

#include "TreeProduct.h"
#include "Payoff.h"
#include "Types.h"

class EuropeanOption : public TreeProduct {
public:
    EuropeanOption(){};
    EuropeanOption(OptionType _optType, double _strike, const Date& _expiry) :optType(_optType), strike(_strike), expiryDate(_expiry) {};
  //   EuropeanOption(OptionType _optType, double _strike, const Date& _expiry, string _underlying): 
  // optType(_optType), strike(_strike), expiryDate(_expiry), underlying(_underlying) {}
    
    // Constructor with initialization for all attributes
    EuropeanOption(OptionType optType, double strike, const Date& expiry, const string& underlying)
        : TreeProduct("EuropeanOption", expiry, underlying),  // Assuming TreeProduct constructor is correctly set up to take these parameters
          optType(optType), 
          strike(strike), 
          expiryDate(expiry),
          underlying(underlying) {}

    virtual double Payoff(double S) const { return PAYOFF::VanillaOption(optType, strike, S); }
    virtual const Date& GetExpiry() const { return expiryDate; }
    double getStrike() const { return strike; }
    OptionType getOptionType() const { return optType; }
    virtual double ValueAtNode(double S, double t, double continuation) const { return continuation; }

protected:
    OptionType optType;
    double strike;
    Date expiryDate;
    string underlying;
};

class EuroCallSpread : public EuropeanOption {
public:
    // EuroCallSpread(double _k1, double _k2, const Date& _expiry): strike1(_k1), strike2(_k2), expiryDate(_expiry) { 
    //     assert(_k1 < _k2); 
    //   };
    EuroCallSpread(double _k1, double _k2, const Date& _expiry) : EuropeanOption(Call, _k1, _expiry), strike1(_k1), strike2(_k2) { 
        assert(_k1 < _k2); 
    };
    virtual double Payoff(double S) const { return PAYOFF::CallSpread(strike1, strike2, S); };
    // virtual const Date& GetExpiry() const { return expiryDate; };

private:
    double strike1;
    double strike2;
    // Date expiryDate;
};

#endif
