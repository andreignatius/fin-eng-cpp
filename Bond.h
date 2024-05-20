#pragma once
#include "Trade.h"


class Bond : public Trade {
public:
    Bond(const std::string& name, const Date& start, const Date& end, double _notional, double _price)
        : Trade("BondTrade", start), bondName(name), startDate(start), maturityDate(end), notional(_notional), tradePrice(_price) {
        // Debugging statements
        //std::cout << "Bond Constructor: Start Date: " << startDate.toString() << ", Maturity Date: " << maturityDate.toString() << std::endl;
    }

    double Payoff(double marketPrice) const override {
        return notional / 100 * (marketPrice - tradePrice);
        cout << marketPrice << endl;
    }
    std::string getBondName() const {
        return bondName;
    }
    std::string getBondInfo() const {
        std::ostringstream oss;
        oss << "Bond Info: Bond Name: " << bondName
            << ", Start Date: " << startDate.toString()
            << ", Maturity Date: " << maturityDate.toString()
            << ", Notional: " << notional
            << ", Trade Price: " << tradePrice;
        return oss.str();
    }

private:
    std::string bondName;
    double notional;
    double tradePrice;
    Date startDate;
    Date maturityDate;
};

