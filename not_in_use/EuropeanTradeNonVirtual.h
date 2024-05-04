#ifndef _EUROPEAN_TRADE
#define _EUROPEAN_TRADE

#include <cassert> 

#include "TreeProductNonVirtual.h"

enum OptionType {Call, Put, BinaryCall, BinaryPut};

class EuropeanOption : public TreeProduct
{
 public:
  double Payoff(double S) const;
  virtual const Date& GetExpiry() const;
  EuropeanOption(OptionType _optType, double _strike, const Date& _expiry);
 private:
  OptionType optType;
  double strike;
  Date expiryDate;  
};

class EuroCallSpread : public TreeProduct
{
 public:
  double Payoff(double S) const;
  virtual const Date& GetExpiry() const;
  EuroCallSpread(double _k1, double _k2, const Date& _expiry)
   : strike1(_k1), strike2(_k2), expiryDate(_expiry)
  {
    assert(_k1 < _k2);
  };
 private:
  double strike1;
  double strike2;
  Date expiryDate;
};

#endif
