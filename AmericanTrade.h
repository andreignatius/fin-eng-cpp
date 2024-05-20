#ifndef _AMERICAN_TRADE
#define _AMERICAN_TRADE

#include <cassert> 

#include "TreeProduct.h"
#include "Types.h"
#include "Payoff.h"

class AmericanOption : public TreeProduct {
public:
  //AmericanOption(const string& name, OptionType _optType, double _strike, const Date& _expiry):
  //asset(name), optType(_optType), strike(_strike), expiryDate(_expiry) {}
  AmericanOption(const std::string& name, OptionType _optType, double _strike, const Date& _expiry)
        : TreeProduct(name), optType(_optType), strike(_strike), expiryDate(_expiry) {}



  virtual double Payoff(double S) const 
  { 
    return PAYOFF::VanillaOption(optType, strike, S); 
  }
  virtual const Date& GetExpiry() const 
  { 
    return expiryDate;
  }
  virtual double ValueAtNode(double S, double t, double continuation) const 
  { 
    return std::max(Payoff(S), continuation); 
  }

  // Inline public member function to return optType
  OptionType getOptionType() const { return optType; }
  // Getter for strike price
  double getStrike() const { return strike; }

  // Getter for expiry date
  const Date& getExpiryDate() const { return expiryDate; }


private:
  string asset;
  OptionType optType;
  double strike;
  Date expiryDate;  
};

class AmerCallSpread : public AmericanOption {
public:
    AmerCallSpread(const std::string& name, double _k1, double _k2, const Date& _expiry)
        : AmericanOption(name, Call, _k1, _expiry), strike1(_k1), strike2(_k2) {
        assert(_k1 < _k2);
    }


    virtual double Payoff(double S) const
    {
      return PAYOFF::CallSpread(strike1, strike2, S);
    }
    virtual const Date& GetExpiry() const
    {
      return expiryDate;
    }
    //// Getter for expiry date
    //const Date& getExpiryDate() const { return expiryDate; }
    // Getter for the first strike price
    double getStrike1() const { return strike1; }

    // Getter for the second strike price
    double getStrike2() const { return strike2; }
private:
  string asset;
  double strike1;
  double strike2;
  Date expiryDate;
};

#endif
