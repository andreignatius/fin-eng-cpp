#ifndef MARKET_H    // this macro is to avoid duplicated include
#define MARKET_H

#include <iostream>
#include "Date.h"   // we define our Date type in a separate class

class Market
{
 public:
  datetime_t asof;
  datetime_t* pp;
  double stockPrice;
  double volatility;
  double interestRate;
  void Print();
  Market(const Market&);
  Market(){};
  ~Market();
  Market& operator=(const Market&);
};

std::ostream& operator<<(std::ostream& os, const Market& obj);
std::istream& operator>>(std::istream& is, Market& obj);

#endif
