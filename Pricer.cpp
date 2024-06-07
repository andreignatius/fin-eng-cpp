#include "Pricer.h"
#include "AmericanTrade.h"
#include "EuropeanTrade.h"
#include "Market.h"
#include <cmath>

double Pricer::Price(const Market &mkt, Trade *trade) {
    double pv = 0.0;

    if (auto *option = dynamic_cast<TreeProduct*>(trade)) {
        // Simplifies handling for both American and European options as both are TreeProduct.
        std::string opt_type_str = OptionTypeToString(option->getOptionType());
        std::cout << "Processing option trade: " << opt_type_str << " " << option->getUnderlying() << std::endl;
        pv = PriceTree(mkt, *option);
    } else {
        double marketPrice = 0.0;
        if (trade->getType() == "BondTrade") {
            marketPrice = mkt.getBondPrice(trade->getUnderlying());
        } else if (trade->getType() == "SwapTrade") {
            std::cout << "Processing swap trade with no relevant market price." << std::endl;
            marketPrice = 0.0;
        } else {
            marketPrice = mkt.getSpotPrice(trade->getUnderlying());
        }
        pv = trade->Payoff(marketPrice);
    }

    return pv;
}


double BinomialTreePricer::PriceTree(const Market &mkt,
                                     const TreeProduct &trade) {
    // model setup
    // double T = trade.GetExpiry() - mkt.asOf;
    double T = trade.GetExpiry().differenceInDays(mkt.asOf) / 365.25;
    double dt = T / nTimeSteps;
    /*
    get these data for the deal from market object
    */
    double stockPrice = 0;
    if (trade.getType() == "TreeProduct" || trade.getType() == "AmericanOption" || trade.getType() == "EuropeanOption") {
      // std::cout << "underlying111: " << trade.getUnderlying() << std::endl;
      stockPrice = mkt.getSpotPrice(trade.getUnderlying());
    } else {
      stockPrice = mkt.getSpotPrice(trade.getType());
    }
    double vol= mkt.getVolCurve("EuropeanOption").getVol(trade.GetExpiry()); 
    double rate = mkt.getRiskFreeRate();

    std::cout << "Tree pricer parameters "<<std::endl;
    std::cout << "T: " << T <<", Price: "<< stockPrice  << ", Vol: "<< vol << ", r: "<<rate<<std::endl;

    ModelSetup(stockPrice, vol, rate, dt);

    for (int i = 0; i <= nTimeSteps; i++) {
        states[i] = trade.Payoff(GetSpot(nTimeSteps, i));
    }

    // price by backward induction
    for (int k = nTimeSteps - 1; k >= 0; k--)
        for (int i = 0; i <= k; i++) {
            // calculate continuation value
            double df = exp(-rate * dt);
            double continuation =
                df * (states[i] * GetProbUp() + states[i + 1] * GetProbDown());
            // calculate the option value at node(k, i)
            states[i] = trade.ValueAtNode(GetSpot(k, i), dt * k, continuation);
        }

    return states[0];
}

void CRRBinomialTreePricer::ModelSetup(double S0, double sigma, double rate,
                                       double dt) {
    double b = std::exp((2 * rate + sigma * sigma) * dt) + 1;
    u = (b + std::sqrt(b * b - 4 * std::exp(2 * rate * dt))) / 2 /
        std::exp(rate * dt);
    p = (std::exp(rate * dt) - 1 / u) / (u - 1 / u);
    currentSpot = S0;
}

void JRRNBinomialTreePricer::ModelSetup(double S0, double sigma, double rate,
                                        double dt) {
    u = std::exp((rate - sigma * sigma / 2) * dt + sigma * std::sqrt(dt));
    d = std::exp((rate - sigma * sigma / 2) * dt - sigma * std::sqrt(dt));
    p = (std::exp(rate * dt) - d) / (u - d);
    currentSpot = S0;
}
