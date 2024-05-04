#include "EuropeanTradeNonVirtual.h"

EuropeanOption::EuropeanOption(OptionType _optType, double _strike, const Date& _expiry)
  : optType(_optType), strike(_strike), expiryDate(_expiry) {}


double EuropeanOption::Payoff(double S) const
{
  switch (optType)
    {
    case Call:
      return S > strike ? S - strike : 0;
    case Put:
      return S < strike ? strike - S : 0;
    case BinaryCall:
      return S >= strike ? 1 : 0;
    case BinaryPut:
      return S <= strike ? 1 : 0;
    default:
      throw "unsupported optionType";
    }
}


const Date& EuropeanOption::GetExpiry() const
{
  return expiryDate;
} 

const Date& EuroCallSpread::GetExpiry() const
{
  return expiryDate;
}


double EuroCallSpread::Payoff(double S) const
{
  if (S < strike1)
    return 0;
  else if (S > strike2)
    return 1;
  else
    return (S - strike1) / (strike2 - strike1);
}

