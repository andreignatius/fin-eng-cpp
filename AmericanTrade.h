#ifndef _AMERICAN_TRADE
#define _AMERICAN_TRADE

#include "Market.h"
#include "Payoff.h" // Assuming necessary for PAYOFF namespace functions
#include "Pricer.h"
#include "TreeProduct.h"
#include "Types.h"
#include "Utils.h"
#include <cassert>

class AmericanOption : public TreeProduct {
  public:
    AmericanOption();
    AmericanOption(OptionType _optType, double _strike, const Date &_expiry);
    AmericanOption(OptionType optType, double strike, const Date &expiry,
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

class AmerCallSpread : public TreeProduct {
  public:
    AmerCallSpread(double k1, double k2, const Date &expiry, const Date &date,
                   const string &uuid);
    virtual double Payoff(double S) const override;
    virtual const Date &GetExpiry() const override;

  private:
    double strike1;
    double strike2;
    Date expiryDate;
    Date valueDate;
    string uuid;
};

#endif
