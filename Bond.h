#pragma once
#include "Trade.h"

class Bond : public Trade {
public:
	Bond(Date start, Date end, double _notional, double _price): Trade("BondTrade", start) {
        notional = _notional;
        tradePrice = _price;
    }

    Bond(Date start, Date end, double _notional, double _price, double _couponRate, string _underlying):
        Trade("BondTrade", start, _underlying), notional(_notional), tradePrice(_price),
        startDate(start), maturityDate(end), couponRate(_couponRate), underlying(_underlying) {}

    double Payoff(double marketPrice) const override;

private:
    double notional;
    double tradePrice;
    double couponRate;  // New attribute
    Date startDate;
    Date maturityDate;
    string underlying;
};