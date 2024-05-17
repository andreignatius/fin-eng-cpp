#pragma once
#include "Trade.h"

// class Bond : public Trade {
// public:
//     Bond(Date start, Date end, double _notional, double _price): Trade("BondTrade", start) {
//         notional = _notional;
//         tradePrice = _price;
//     }

//     inline double Payoff(double marketPrice) const { return 0;}; // implement this

// private:
//     double notional;
//     double tradePrice;
//     Date startDate;
//     Date maturityDate;
// };

class Bond : public Trade {
public:
	Bond(Date start, Date end, double _notional, double _price): Trade("BondTrade", start) {
        notional = _notional;
        tradePrice = _price;
    }
    
    Bond(Date start, Date end, double _notional, double _price, double _couponRate):
        Trade("BondTrade", start), notional(_notional), tradePrice(_price),
        startDate(start), maturityDate(end), couponRate(_couponRate) {}

    inline double Payoff(double marketPrice) const override {
        double couponPayment = notional * couponRate;
        double redemptionValue = notional + couponPayment;  // Simplified example
        double priceDifference = marketPrice - tradePrice;
        return (redemptionValue - tradePrice) * notional;
    }

private:
    double notional;
    double tradePrice;
    double couponRate;  // New attribute
    Date startDate;
    Date maturityDate;
};