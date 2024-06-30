#pragma once
#include "Date.h"
#include "Market.h"
#include "Trade.h"

class Swap : public Trade {
  public:
    // make necessary change
    //  Swap(Date start, Date end, double _notional, double _price):
    //  Trade("SwapTrade", start) {
    //      /*

    //     add constructor details

    //     */
    // }
    // Swap(Date start, Date end, double _notional, double _tradeRate, double
    // _frequency, bool _isFixedForFloating, const Market& _market);
    Swap(Date start, Date end, Date _valueDate, double _notional,
         double _fixedRate, double _frequency, bool _isFixedForFloating,
         const Market &_market, const string _curveName, const string _uuid)
        : Trade("SwapTrade", start, _valueDate, _curveName, _uuid), startDate(start),
          maturityDate(end), valueDate(_valueDate), notional(_notional),
          fixedRate(_fixedRate), frequency(_frequency),
          isFixedForFloating(_isFixedForFloating), market(_market),
          curveName(_curveName), uuid(_uuid) {
        if (startDate >= maturityDate) {
            throw std::invalid_argument(
                "Start date must be before the maturity date.");
        }
        if (fixedRate < 0) {
            throw std::invalid_argument("Fixed rate cannot be negative.");
        }
        if (frequency <= 0) {
            std::cout << "check frequency" << frequency << "!" << std::endl;
            throw std::invalid_argument(
                "Frequency must be a positive integer.");
        }
    }
    /*
    implement this, using pv = annuity * (trade rate - market rate);
    approximately, annuity = sum of (notional * Discount factor at each period
    end); for a 2 year swap, with annual freq and notinal 100, annuity = 100 *
    Df(at year 1 end) + 100* Df(at year 2 end); Df = exp(-rT), r taken from
    curve;
    */
    virtual double Payoff(double marketPrice) const override; // use Market data
    std::string toString() const;
    double getAnnuity() const; // implement this in a cpp file

  private:
    Date startDate;
    Date maturityDate;
    Date valueDate;
    double notional;
    double fixedRate;
    double tradeRate;
    double frequency;        // use 1 for annual, 0.5 for semi-annual etc
    bool isFixedForFloating; // true if fixed-for-floating swap, false otherwise
    const Market &market;    // Store a reference to Market
    string curveName;
    string uuid;
};