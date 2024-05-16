#ifndef _TREE_PRODUCT_H
#define _TREE_PRODUCT_H
#include "Date.h"
#include "Trade.h"

//option type of trade, will be priced using tree model
class TreeProduct: public Trade
{
public:
    TreeProduct(): Trade() { tradeType = "TreeProduct";};
    // Adding a constructor that initializes tradeType, tradeDate, and underlying
    TreeProduct(const string& _type, const Date& _expiry, const string& _underlying)
        : Trade(_type, _expiry, _underlying) {}
    virtual const Date& GetExpiry() const = 0;
    virtual double ValueAtNode(double stockPrice, double t, double continuationValue) const = 0;
};

#endif
