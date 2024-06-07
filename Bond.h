#pragma once
#include "Trade.h"

class Bond : public Trade {
public:
	Bond(Date start, Date end, double _notional, double _price): Trade("BondTrade", start) {
        notional = _notional;
        tradePrice = _price;
    }

    Bond(Date start, Date end, double _notional, double _price, double _couponRate, string _underlying, string _uuid):
        Trade("BondTrade", start, _underlying, _uuid), notional(_notional), tradePrice(_price),
        startDate(start), maturityDate(end), couponRate(_couponRate), underlying(_underlying), uuid(_uuid) {}

    double Payoff(double marketPrice) const override;
    std::string toString() const;

private:
    double notional;
    double tradePrice;
    double couponRate;  // New attribute
    Date startDate;
    Date maturityDate;
    string underlying;
    string uuid;
};