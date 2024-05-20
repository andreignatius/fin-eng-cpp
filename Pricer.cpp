#include <cmath>
#include "Pricer.h"
#include <iostream>

#include <ctime>
#include "Types.h" 

//#include "EuropeanTrade.h"
//#include "AmericanTrade.h"


double Pricer::Price(const Market& mkt, Trade* trade) {

  double pv;
  double price;
  if (trade->getType() == "TreeProduct") {
    TreeProduct* treePtr = dynamic_cast<TreeProduct*>(trade);
    if (treePtr) { //check if cast is sucessful
      pv = PriceTree(mkt, *treePtr);
    }
  }
  else if(trade->getType() == "BondTrade"){
      Bond* bondPtr = dynamic_cast<Bond*>(trade);
      if (bondPtr) { //check if cast is sucessful

          const std::unordered_map<std::string, double>& mktBonds = mkt.getAllBondPrices();
          string bondName = bondPtr->getBondName();
          //cout << "start pricing with bond pricer for " << bondName << endl;

          auto it = mktBonds.find(bondName);
          if (it != mktBonds.end()) {
              price = it->second;
              //cout << "Bond Name: " << bondName << ", Price: " << price << endl;
          }
          else {
              cout << "Bond with name " << bondName << " not found." << endl;
              cout << "Market Price defaulted to 0!" << endl;
              price = 0;
          }
          
          pv = bondPtr->Payoff(price);
          return pv;
      }

  }
  else if (trade->getType() == "SwapTrade") {
      Swap* swapPtr = dynamic_cast<Swap*>(trade);
      if (swapPtr) { //check if cast is sucessful

          const RateCurve& mktRates = mkt.getCurve(swapPtr->getCurveName());
          string swapName = swapPtr->getCurveName();
          cout << "start pricing with swap pricer for " << swapName << endl;

          //cout << mktRates.getName() << endl;
          if (swapName != mktRates.getName()) {
              cout << "No market curve matched for " << swapName << ", will price using default curve USD-SOFR!" << endl;
              cout << mktRates.getName() << endl;
  
          }


          double marketRate = swapPtr->getMktRate(mktRates);
          double annuity = swapPtr->getAnnuity(mktRates);


          pv = swapPtr->Payoff(marketRate, annuity);
          return pv;

      }
  else {
          double price = 0.; //get from market data
          pv = trade->Payoff(price);
  }

      return pv;
  }
}

double BinomialTreePricer::PriceTree(const Market& mkt, const TreeProduct& trade){
  // model setup
  double T = trade.GetExpiry() - mkt.asOf;
  double dt = T / nTimeSteps;

  RateCurve rateC = mkt.getCurve("USD-SOFR");
  VolCurve volC = mkt.getVolCurve("volCurve");

  double rate = rateC.getRate(trade.GetExpiry());
  double vol = volC.getVol(trade.GetExpiry());
  double stockPrice=0;

  cout << trade.getAssetName() << endl;
  /*
  get these data for the deal from market object
  */

  // Find the stock price for the asset
  const auto& stocks = mkt.getAllStockPrices();

  //for (const auto& pair : stocks) {
  //    cout << "Asset: " << pair.first << ", Price: " << pair.second << endl;
  //}


  auto it = stocks.find(trade.getAssetName());
  if (it != stocks.end()) {
      stockPrice = it->second;
      //cout << "I've found the current market price: " << stockPrice << endl;
  }
  else {
      std::cerr << "Warning: Stock price not found for asset: " << trade.getAssetName() << ". Defaulting to 0." << std::endl;
      return 0;
  }

  cout << "Model setup as follows:" << endl;
  cout << "S0=" << stockPrice << ", vol=" << vol << ", r=" << rate << ", T=" << T << ", steps=" << nTimeSteps << endl;

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
  cout << "pricing using CRR tree" << endl;
  double b = std::exp((2*rate+sigma*sigma)*dt)+1;
  u = (b + std::sqrt(b*b - 4*std::exp(2*rate*dt))) / 2 / std::exp(rate*dt);
  p = (std::exp(rate*dt) -  1/u) / (u - 1/u);
  currentSpot = S0;
}

void JRRNBinomialTreePricer::ModelSetup(double S0, double sigma, double rate, double dt)
{
    cout << "pricing using JRRN tree" << endl;
    u = std::exp( (rate - sigma*sigma/2) * dt + sigma * std::sqrt(dt) );
    d = std::exp( (rate - sigma*sigma/2) * dt - sigma * std::sqrt(dt) );
    p = (std::exp(rate*dt) -  d) / (u - d);
    currentSpot = S0;
}

