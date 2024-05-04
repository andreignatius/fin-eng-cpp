#ifndef _TREE_PRICER
#define _TREE_PRICER

#include <vector>
#include <cmath>

#include "TreeProductNonVirtual.h"

double binomialTree ( const Market& market, const TreeProduct& trade, int N)
{
  double T = trade.GetExpiry() - market.asof;
  double sigma = market.volatility;
  double rate = market.interestRate;
  std::vector<double> states(N+1);
  double dt = T / N;
  double b = std::exp((2*rate+sigma*sigma)*dt)+1;
  double u = (b + std::sqrt(b*b - 4*std::exp(2*rate*dt))) / 2 / std::exp(rate*dt);
  double p = (std::exp(rate*dt) -  1/u) / (u - 1/u);
  // initialize the final states, apply payoff directly
  for (int i = 0; i <= N; i++) {
    double S = market.stockPrice * std::pow(u, N-2*i);
    states[i] = trade.Payoff(S);
  }
  for (int k = N-1; k >= 0; k--)
    for (int i = 0; i <= k; i++)
      states[i] = states[i]*p + states[i+1]*(1-p);

  return exp(-rate*T) * states[0];
}

#endif
