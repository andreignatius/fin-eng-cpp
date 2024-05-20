#ifndef _TREE_PRODUCT_H
#define _TREE_PRODUCT_H
#include "Date.h"
#include "Trade.h"

//option type of trade, will be priced using tree model
class TreeProduct: public Trade
{
public:
    //TreeProduct(): Trade() { tradeType = "TreeProduct";};

    TreeProduct() : Trade(), asset("") { tradeType = "TreeProduct"; }
    TreeProduct(const std::string& assetName) : Trade(), asset(assetName) { tradeType = "TreeProduct"; }

    virtual const Date& GetExpiry() const = 0;
    virtual double ValueAtNode(double stockPrice, double t, double continuationValue) const = 0;

    // Getter for asset name
    string getAssetName() const { return asset; }
private:
    string asset;

};

#endif
