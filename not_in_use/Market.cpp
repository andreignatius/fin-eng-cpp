#include "Market.h"

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
