#ifndef _EUROPEAN_TRADE
#define _EUROPEAN_TRADE

#include "Market.h"
#include "Payoff.h" // Include if PAYOFF namespace functions are defined here
#include "Pricer.h"
#include "TreeProduct.h"
#include "Types.h"
#include "Utils.h"
#include <cassert>

class EuropeanOption : public TreeProduct {
  public:
    EuropeanOption();
    EuropeanOption(OptionType _optType, double _strike, const Date &_expiry);
    EuropeanOption(OptionType optType, double strike, const Date &expiry,
                   const Date &date, const string &underlying,
                   const string &uuid);

    virtual double Payoff(double S) const override;
    virtual const Date &GetExpiry() const override;
    double getStrike() const;
    OptionType getOptionType() const;
    virtual double ValueAtNode(double S, double t,
                               double continuation) const override;
    std::vector<double> CalculateDV01(const Market &market,
                                      const Date &valueDate,
                                      Pricer *pricer) const;

    std::vector<double> CalculateVega(const Market &market,
                                      const Date &valueDate,
                                      Pricer *pricer) const;
    std::string toString() const;

  private:
    OptionType optType;
    double strike;
    Date expiryDate;
    Date valueDate; // doubles as curve date selector
    string underlying;
    string uuid;
};

class EuroCallSpread : public EuropeanOption {
  public:
    EuroCallSpread(double _k1, double _k2, const Date &_expiry,
                   const Date &_date, const string &_uuid);

    virtual double Payoff(double S) const override;

  private:
    double strike1;
    double strike2;
    string uuid;
};

#endif // _EUROPEAN_TRADE
