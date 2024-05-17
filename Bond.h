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
        startDate(start), maturityDate(end), couponRate(_couponRate), underlying(_underlying) {
        	std::cout << "999create bond with name :" << _underlying << std::endl;
        }

    inline double Payoff(double marketPrice) const override {
	    std::cout << "*bondPrice: " << marketPrice << std::endl;
	    double priceDifference = (marketPrice - tradePrice) / 100;  // Adjusted for bond pricing
	    double payoff = priceDifference * notional;
	    std::cout << "notional :" << notional << " tradePrice: " << tradePrice << " marketPrice: " << marketPrice << std::endl;
	    std::cout << "@compute payoff:" << payoff << std::endl;
	    return payoff;
	}

private:
    double notional;
    double tradePrice;
    double couponRate;  // New attribute
    Date startDate;
    Date maturityDate;
    string underlying;
};