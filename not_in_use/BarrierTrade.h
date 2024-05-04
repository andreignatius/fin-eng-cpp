#ifndef BARRIER_TRADE
#define BARRIER_TRADE

#include "TreeProduct.h"
#include "EuropeanTrade.h"
#include "Barrier.h"

class BarrierTrade : public TreeProduct
{
 public:
  BarrierTrade(const Barrier& bar) : barrier(bar) {}
  virtual double ValueAtNode(double S, double t, double continuation) const
  {
    return barrier.BarrierPayoff(S, t, continuation);
  }
  
 protected:
  IBarrier barrier;
};

class BarrierOption : public BarrierTrade
{
 public:
  // constructor
  BarrierOption(const Barrier& bar, const EuropeanOption& opt)
    : option(opt), BarrierTrade(bar) {}

  // Implementing interface of TreeProduct
  virtual const Date& GetExpiry() const
  {
    return option.GetExpiry();
  }
  virtual double Payoff(double S) const
  {
    return option.Payoff(S);
  }
 private:
  EuropeanOption option;
};

class NoTouch : public BarrierTrade
{
 public:
  // constructor
  NoTouch(double _payoff, const Barrier& bar, const Date& exp)
    :payoff(_payoff), BarrierTrade(bar), expiryDate(exp) {}

  // Implementing interface of TreeProduct
  virtual double Payoff(double S) const
  {
    return payoff;
  }
  virtual const Date& GetExpiry() const
  {
    return expiryDate;
  }

 private:
  double payoff;
  Date expiryDate;
};

#endif
