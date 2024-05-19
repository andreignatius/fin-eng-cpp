#pragma once
#include <string>
#include "Date.h"

using namespace std;

class Trade {
public:
    Trade(){};
    Trade(const string& _type, const Date& _tradeDate): tradeType(_type), tradeDate(_tradeDate) {};
    Trade(const string& _type, const Date& _tradeDate, const string& _underlying)
        : tradeType(_type), tradeDate(_tradeDate), underlying(_underlying) {}

    inline string getType() const { return tradeType; }
    inline string getUnderlying() const { return underlying; } // Accessor for the underlying asset
    virtual double Payoff(double marketPrice) const = 0;
    virtual ~Trade() {};

protected:   
    string tradeType;
    Date tradeDate;
private:
    string underlying; // Identifier for the underlying asset
};
