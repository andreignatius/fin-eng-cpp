#include "Pricer.h"
#include "AmericanTrade.h"
#include "EuropeanTrade.h"
#include "Market.h"
#include "Constants.h"
#include <cmath>

double Pricer::Price(const Market &mkt, Trade *trade) {
    double pv = 0.0;

    if (auto *option = dynamic_cast<TreeProduct*>(trade)) {
        // Simplifies handling for both American and European options as both are TreeProduct.
        // verbose logic below for reference / trace of code for debugging purposes
        pv = PriceTree(mkt, *option);
    } else {
        double marketPrice = 0.0;
        if (trade->getType() == "BondTrade") {
            marketPrice = mkt.getBondPrice(trade->getUnderlying());
            std::cout << "bond px: " << marketPrice << std::endl;
        } else if (trade->getType() == "SwapTrade") {
            std::cout << "Processing swap trade with no relevant market price." << std::endl;
            marketPrice = 0.0;
        } else {
            marketPrice = mkt.getSpotPrice(trade->getUnderlying(), Date(2024, 6, 1));
        }
        pv = trade->Payoff(marketPrice);
    }

    return pv;
}

double Pricer::CalculateDV01(const Market& market, Trade* trade) {
    Market perturbedMarket = market;
    perturbedMarket.adjustInterestRates(0.0001);  // Increase all rates by 1 bps
    double priceOriginal = Price(market, trade);
    double pricePerturbed = Price(perturbedMarket, trade);
    std::cout << std::fixed << std::setprecision(10);  // Set precision for better visibility of small changes
    std::cout << "price original: " << priceOriginal << ", pricePerturbed: " << pricePerturbed << std::endl;
    return pricePerturbed - priceOriginal;
}

double Pricer::CalculateVega(const Market& market, Trade* trade) {
    double vega = 0.0;
    Market perturbedMarket = market;

    if (auto* amerOption = dynamic_cast<AmericanOption*>(trade)) {
        perturbedMarket.adjustVolatility(amerOption->getUnderlying(), 0.01);  // Increase vol by 1%
        double priceOriginal = Price(market, amerOption);
        double pricePerturbed = Price(perturbedMarket, amerOption);
        vega = pricePerturbed - priceOriginal;
    } else if (auto* euroOption = dynamic_cast<EuropeanOption*>(trade)) {
        perturbedMarket.adjustVolatility(euroOption->getUnderlying(), 0.01);  // Increase vol by 1%
        double priceOriginal = Price(market, euroOption);
        double pricePerturbed = Price(perturbedMarket, euroOption);
        vega = pricePerturbed - priceOriginal;
    }
    
    return vega;
}



// REFERENCE : VERBOSE LOGIC FOR DEBUGGING PURPOSES
// double Pricer::Price(const Market &mkt, Trade *trade) {
//     double pv;
//     // std::cout<< "mkt: " << mkt << " trade!!!: " << trade->getType() << std::endl;
//     if (trade->getType() == "AmericanOption" || trade->getType() == "EuropeanOption") {
//         std::string opt_type_str = "";
//         std::string underlying = "";
//         Date expiry ;
//         double strike= 0 ;
//         // std::cout << "tree product" << " " << trade << std::endl;
//         if (trade->getType() == "AmericanOption"){
//             AmericanOption *tempptr = dynamic_cast<AmericanOption *>(trade);
//             OptionType opt_type = tempptr->getOptionType();
//             if (opt_type == Call){
//                 opt_type_str = "CALL";
//             } else if (opt_type == Put){
//                 opt_type_str = "PUT";
//             }            
//             strike = tempptr ->getStrike();
//             underlying = tempptr->getUnderlying();
//             expiry = tempptr->GetExpiry();

//         } else if (trade->getType() == "EuropeanOption"){
//             EuropeanOption *tempptr = dynamic_cast<EuropeanOption *>(trade);
//             OptionType opt_type = tempptr->getOptionType();
//             if (opt_type == Call){
//                 opt_type_str = "CALL";
//             } else if (opt_type == Put){
//                 opt_type_str = "PUT";
//             }
//             strike = tempptr ->getStrike();
//             underlying = tempptr->getUnderlying();
//             expiry = tempptr->GetExpiry();
//         }
//         TreeProduct *treePtr = dynamic_cast<TreeProduct *>(trade);
//         if (treePtr) { // check if cast is sucessful
//             pv = PriceTree(mkt, *treePtr);
//         }
//     } else {
//         double price; // get from market data
//         if (trade->getType() == "BondTrade") {
//             // std::cout << "check bond name : " << trade->getUnderlying() << std::endl;
//             price = mkt.getBondPrice(trade->getUnderlying());
//         }
//         else if (trade->getType() == "SwapTrade"){
//             std::cout<<"This is a SWAP TRADE"<<std::endl;
//             price = 0;
//         } else {
//             price = mkt.getSpotPrice(trade->getUnderlying());
//         }
        
//         // std::cout << "not tree product, where spot price : " << price << " for underlying : " << trade->getUnderlying() << endl;
//         pv = trade->Payoff(price); // should be noted that for Swap , market price input is irrelevant
//     }
//     return pv;
// }


double BinomialTreePricer::PriceTree(const Market &mkt,
                                     const TreeProduct &trade) {
    // model setup
    // double T = trade.GetExpiry() - mkt.asOf;
    double T = trade.GetExpiry().differenceInDays(mkt.asOf) / Constants::NUM_DAYS_IN_YEAR;
    double dt = T / nTimeSteps;
    /*
    get these data for the deal from market object
    */
    double stockPrice = 0;
    if (trade.getType() == "TreeProduct" || trade.getType() == "AmericanOption" || trade.getType() == "EuropeanOption") {
      // std::cout << "underlying111: " << trade.getUnderlying() << std::endl;
      stockPrice = mkt.getSpotPrice(trade.getUnderlying(), Date(2024, 6, 1));
    } else {
      stockPrice = mkt.getSpotPrice(trade.getType(), Date(2024, 6, 1));
    }
    double vol= mkt.getVolCurve(Date(2024, 6, 1), "EuropeanOption").getVol(trade.GetExpiry()); 
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
