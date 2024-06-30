#pragma once
#include "Market.h"
#include "Trade.h"
#include <vector>
class Bond : public Trade {
  public:
    /*
    Bond(Date start, Date end, double _notional, double _price)
        : Trade("BondTrade", start) {
        notional = _notional;
        tradePrice = _price;
    }

    Bond(Date start, Date end, double _notional, double _price,
         double _couponRate, string _underlying, string _uuid)
        : Trade("BondTrade", start, _underlying, _uuid), notional(_notional),
          tradePrice(_price), startDate(start), maturityDate(end),
          couponRate(_couponRate), underlying(_underlying), uuid(_uuid) {}
    */

    Bond(Date start, Date end, Date _valueDate, double _notional,
         double _couponRate, double _frequency, const Market &_market,
         const string _curveName, const string _uuid)
        : Trade("BondTrade", start, _valueDate, _curveName, _uuid), startDate(start),
          maturityDate(end), valueDate(_valueDate), notional(_notional),
          couponRate(_couponRate), frequency(_frequency), market(_market),
          curveName(_curveName), uuid(_uuid) {
        std::cout << "init bond trade : " << start.toString() << "," << end.toString() << "," << _valueDate.toString()
        << "," << _notional << "," << _couponRate << "," << _frequency << "," << _curveName << "," << _uuid << std::endl;
        if (startDate >= maturityDate) {
            throw std::invalid_argument(
                "Start date must be before the maturity date.");
        }
        if (couponRate < 0) {
            throw std::invalid_argument("Coupon Rate cannot be negative.");
        }
        if (frequency <= 0) {
            std::cout << "check frequency" << frequency << "!" << std::endl;
            throw std::invalid_argument("Frequency must be a positive.");
        }
    }

    double Payoff(double marketPrice) const override;
    std::string toString() const;

  private:
    double notional;
    double tradePrice;
    double couponRate;    // New attribute
    double frequency;     // use 1 for annual, 0.5 for semi-annual etc
    const Market &market; // Store a reference to Market
    Date startDate;
    Date maturityDate;
    Date valueDate; // doubles as curve date selector
    string curveName;
    string uuid;
};