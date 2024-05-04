#include "MarketTemp.h"

void Market::Print()
{
  std::cout << "market asof:\t" << asof
	    << "stock price:\t" << stockPrice << std::endl
	    << "volatility: \t" << volatility << std::endl
	    << "interestRate: \t" << interestRate << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Market& mkt)
{
  os << mkt.asof << std::endl
     << mkt.stockPrice << std::endl
     << mkt.volatility << std::endl
     << mkt.interestRate << std::endl;
  return os;
}

std::istream& operator>>(std::istream& is, Market& mkt)
{
  is >> mkt.asof
     >> mkt.stockPrice
     >> mkt.volatility
     >> mkt.interestRate;
  return is;
}

Market::Market(const Market& mkt)
{
  asof = mkt.asof;
  pp = new datetime_t(*mkt.pp);
  stockPrice = mkt.stockPrice;
  volatility = mkt.volatility;
  interestRate = mkt.interestRate;
}

Market::~Market()
{
  if (pp != NULL)
    delete pp;
}

Market& Market::operator=(const Market& mkt)
{
  if (this == &mkt)
    return *this;
  asof = mkt.asof;
  if (pp == NULL)
    pp = new datetime_t(*mkt.pp);
  else
    *pp = *mkt.pp;  
  stockPrice = mkt.stockPrice;
  volatility = mkt.volatility;
  interestRate = mkt.interestRate;
  return *this;
}
