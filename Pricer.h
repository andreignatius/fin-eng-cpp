#ifndef _PRICER
#define _PRICER

#include <vector>
#include <cmath>

#include "Trade.h"
#include "TreeProduct.h"
#include "Market.h"

#include "Bond.h"
#include "Swap.h"

//interface
class Pricer {
public:
  virtual double Price(const Market& mkt, Trade* trade);

private:
  virtual double PriceTree(const Market& mkt, const TreeProduct& trade) { return 0; };
};


//class BlackScholesPricer: public Pricer {
//public:
//    // Function to calculate the cumulative normal distribution
//    double cnorm(double x);
//    // Black-Scholes formula for different payoff types
//
//    double bsPrice(double S, double r, double vol, double T, double strike, OptionType payoffType);
//    // Black-Scholes formula for different payoff types
//    //double bsPrice(double S, double r, double vol, double T, double strike, OptionType payoffType);
//
//    double Price(const Market& mkt, Trade* trade);
//
//};



class BinomialTreePricer: public Pricer
{
public:
  BinomialTreePricer(int N) {
    nTimeSteps = N;
    states.resize(N+1);
  }
  double PriceTree(const Market& mkt, const TreeProduct& trade) override;


protected:
  virtual void ModelSetup(double S0, double sigma, double rate, double dt) = 0; // pure virtual
  virtual double GetSpot(int ti, int si) const = 0;
  virtual double GetProbUp() const = 0;
  virtual double GetProbDown() const = 0;
  
private:
  int nTimeSteps;
  std::vector<double> states;
};

class CRRBinomialTreePricer : public BinomialTreePricer
{
public:
  CRRBinomialTreePricer(int N) : BinomialTreePricer(N){}

protected:
  void ModelSetup(double S0, double sigma, double rate, double dt); 
  double GetSpot(int ti, int si) const {
    return currentSpot * std::pow(u, ti-2*si); 
  }
  double GetProbUp() const {return p;}
  double GetProbDown() const {return 1-p;}
  
private:
  double u; // up multiplicative
  double p; // probability for up state
  double currentSpot; // current market spot price
};

class JRRNBinomialTreePricer : public BinomialTreePricer
{
public:
  JRRNBinomialTreePricer(int N) : BinomialTreePricer(N){}
 
protected:
  void ModelSetup(double S0, double sigma, double rate, double dt);
  
  double GetSpot(int ti, int si) const
  {
    return currentSpot * std::pow(u, ti-si) * std::pow(d, si);
  }
  double GetProbUp() const {return p;}
  double GetProbDown() const {return 1-p;}

private:
  double u; // up multiplicative
  double d; // down multiplicative
  double p; // probability for up state
  double currentSpot; // current market spot price
}; 

#endif
