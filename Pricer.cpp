#include <cmath>
#include "Pricer.h"


double Pricer::Price(const Market& mkt, Trade* trade) {
  double pv;
  if (trade->getType() == "TreeProduct") {
    TreeProduct* treePtr = dynamic_cast<TreeProduct*>(trade);
    if (treePtr) { //check if cast is sucessful
      pv = PriceTree(mkt, *treePtr);
    }
  }
  else{
    double price; //get from market data
    pv = trade->Payoff(price);
  }

  return pv;
}

double BinomialTreePricer::PriceTree(const Market& mkt, const TreeProduct& trade) {
  // model setup
  double T = trade.GetExpiry() - mkt.asOf;
  double dt = T / nTimeSteps;
  double stockPrice, vol, rate;
  /*
  get these data for the deal from market object
  */
  ModelSetup(stockPrice, vol, rate, dt);
  
  // initialize
  for (int i = 0; i <= nTimeSteps; i++) {
    states[i] = trade.Payoff( GetSpot(nTimeSteps, i) );
  }    
  
  // price by backward induction
  for (int k = nTimeSteps-1; k >= 0; k--)
    for (int i = 0; i <= k; i++) {
    // calculate continuation value
      double df = exp(-rate *dt);	  
      double continuation = df * (states[i]*GetProbUp() + states[i+1]*GetProbDown());
      // calculate the option value at node(k, i)
      states[i] = trade.ValueAtNode( GetSpot(k, i), dt*k, continuation);
    }

  return states[0];

}

void CRRBinomialTreePricer::ModelSetup(double S0, double sigma, double rate, double dt)
{
  double b = std::exp((2*rate+sigma*sigma)*dt)+1;
  u = (b + std::sqrt(b*b - 4*std::exp(2*rate*dt))) / 2 / std::exp(rate*dt);
  p = (std::exp(rate*dt) -  1/u) / (u - 1/u);
  currentSpot = S0;
}

void JRRNBinomialTreePricer::ModelSetup(double S0, double sigma, double rate, double dt)
{
    u = std::exp( (rate - sigma*sigma/2) * dt + sigma * std::sqrt(dt) );
    d = std::exp( (rate - sigma*sigma/2) * dt - sigma * std::sqrt(dt) );
    p = (std::exp(rate*dt) -  d) / (u - d);
    currentSpot = S0;
}
