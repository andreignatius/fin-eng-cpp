#pragma once
#include "Trade.h"


class Swap : public Trade {
public:
    //make necessary change
    // Constructor
    Swap(const std::string& curveName, Date start, Date end, double _notional, double _tradeRate, int _frequency)
        : Trade("SwapTrade", start), curveName(curveName), startDate(start), maturityDate(end), notional(_notional), tradeRate(_tradeRate)
    {
        // Validate frequency
        valid = true;
        if (_frequency != 1 && _frequency != 2 && _frequency != 3 && _frequency != 4 && _frequency != 6 && _frequency != 12) {
            cout << "Frequency must be one of the values: [1, 2, 3, 4, 6, 12]" << endl;
            valid = false;
        }
        frequency = _frequency;
    }
    
    /*
    implement this, using pv = annuity * (trade rate - market rate);
    approximately, annuity = sum of (notional * Discount factor at each period end);
    for a 2 year swap, with annual freq and notinal 100, annuity = 100 * Df(at year 1 end) + 100* Df(at year 2 end);
    Df = exp(-rT), r taken from curve;
    */
    //inline double Payoff(double marketRate) const { return 0; }; 


    // Override the Payoff method with one parameter
    double Payoff(double marketRate) const override;

    // Additional Payoff method with two parameters
    double Payoff(double marketRate, double annuity) const;

    // Implement getAnnuity method
    double getMktRate(const RateCurve& rates) const;
    double getAnnuity(const RateCurve &rates) const;
    //double getAnnuity() const; //implement this in a cpp file
    // Getter methods for each member variable
    string getCurveName() const { return curveName; }
    Date getStartDate() const { return startDate; }
    Date getMaturityDate() const { return maturityDate; }
    double getNotional() const { return notional; }
    double getTradeRate() const { return tradeRate; }
    int getFrequency() const { return frequency; }
    bool getValid() const { return valid; }
private:
    std::string curveName;
    Date startDate;
    Date maturityDate;
    double notional;
    double tradeRate;
    int frequency; // use 1 for annual, 2 for semi-annual etc
    bool valid;
};