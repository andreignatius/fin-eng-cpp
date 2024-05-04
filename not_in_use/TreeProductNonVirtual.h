#ifndef _TREE_PRODUCT_H
#define _TREE_PRODUCT_H
#include "Date.h"

class TreeProduct
{
 public:
  virtual const Date& GetExpiry() const = 0;
  double Payoff(double stockPrice) const
  {
    throw std::runtime_error("class TreeProduct: not in my job scope");
  }
};
#endif
