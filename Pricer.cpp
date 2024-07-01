#include "Pricer.h"
#include "AmericanTrade.h"
#include "Constants.h"
#include "EuropeanTrade.h"
#include "Market.h"
#include <cmath>

double Pricer::Price(const Market &mkt, const Trade *trade,
                     const Date &valueDate) {
    double pv = 0.0;

    if (auto *option = dynamic_cast<const TreeProduct *>(trade)) {
        // Simplifies handling for both American and European options as both
        // are TreeProduct. verbose logic below for reference / trace of code
        // for debugging purposes
        pv = PriceTree(mkt, *option, valueDate);
        // I NEED TO SEE THE STRIKE !
        auto *americanOption = dynamic_cast<const AmericanOption *>(trade);
        auto *europeanOption = dynamic_cast<const EuropeanOption *>(trade);
        if (americanOption) {
            std::cout << "AMERICAN STRIKE (K) = " << americanOption->getStrike()
                      << std::endl;
        } else if (europeanOption) {
            std::cout << "EURO STRIKE (K) = " << europeanOption->getStrike()
                      << std::endl;
        } else {
            std::cerr << "Cast failed to check OPTION STRIKE" << std::endl;
        }
    } else {
        double marketPrice = 0.0;
        if (trade->getType() == "BondTrade") {
            // std::cout << "checking bond px for " << trade->getUnderlying()
            //           << std::endl;
            marketPrice = mkt.getBondPrice(trade->getUnderlying());
            // std::cout << "bond px: " << marketPrice << std::endl;
        } else if (trade->getType() == "SwapTrade") {
            std::cout << "Processing swap trade with no relevant market price."
                      << std::endl;
            marketPrice = 0.0;
        } else {
            marketPrice = mkt.getPriceOrRate(trade->getUnderlying(), valueDate);
        }
        pv = trade->Payoff(marketPrice);
    }
    return pv;
}

double Pricer::CalculateDV01(const Market &market, Trade *trade,
                             const Date &valueDate) {
    Market perturbedMarket = market;
    perturbedMarket.adjustInterestRates(0.0001); // Increase all rates by 1 bps
    double priceOriginal = Price(market, trade, valueDate);
    double pricePerturbed = Price(perturbedMarket, trade, valueDate);
    std::cout
        << std::fixed
        << std::setprecision(
               10); // Set precision for better visibility of small changes
    std::cout << "price original: " << priceOriginal
              << ", pricePerturbed: " << pricePerturbed << std::endl;
    return pricePerturbed - priceOriginal;
}

double Pricer::CalculateVega(const Market &market, Trade *trade,
                             const Date &valueDate) {
    double vega = 0.0;
    Market perturbedMarket = market;

    if (auto *amerOption = dynamic_cast<AmericanOption *>(trade)) {
        perturbedMarket.adjustVolatility(amerOption->getUnderlying(),
                                         0.01); // Increase vol by 1%
        double priceOriginal = Price(market, amerOption, valueDate);
        double pricePerturbed = Price(perturbedMarket, amerOption, valueDate);
        vega = pricePerturbed - priceOriginal;
    } else if (auto *euroOption = dynamic_cast<EuropeanOption *>(trade)) {
        perturbedMarket.adjustVolatility(euroOption->getUnderlying(),
                                         0.01); // Increase vol by 1%
        double priceOriginal = Price(market, euroOption, valueDate);
        double pricePerturbed = Price(perturbedMarket, euroOption, valueDate);
        vega = pricePerturbed - priceOriginal;
    }

    return vega;
}
double BinomialTreePricer::PriceTree(const Market &mkt,
                                     const TreeProduct &trade,
                                     const Date &valueDate) {
    // model setup
    // double T = trade.GetExpiry() - mkt.asOf;
    double T = trade.GetExpiry().differenceInDays(mkt.asOf) /
               Constants::NUM_DAYS_IN_YEAR;
    double dt = T / nTimeSteps;
    /*
    get these data for the deal from market object
    */
    double stockPrice = 0;
    if (trade.getType() == "TreeProduct" ||
        trade.getType() == "AmericanOption" ||
        trade.getType() == "EuropeanOption") {
        // std::cout << "underlying111: " << trade.getUnderlying() << std::endl;
        stockPrice = mkt.getPriceOrRate(trade.getUnderlying(), valueDate);
        // std::cout << "px output: " << stockPrice << std::endl;

    } else {
        stockPrice = mkt.getPriceOrRate(trade.getType(), valueDate);
    }
    double vol = mkt.getVolCurve(valueDate, trade.getUnderlying())
                     .getVol(trade.GetExpiry());
    // std::cout << "I WANT TO SEE THE VOL CURVE" << std::endl;
    // mkt.getVolCurve(valueDate, trade.getUnderlying()).display();
    double rate =
        mkt.getCurve(valueDate, "usd-sofr").getRate(trade.GetExpiry());
    std::cout << "Tree pricer parameters " << std::endl;
    std::cout << "T: " << T << ", Price: " << ", Strike " << stockPrice
              << ", Vol: " << vol << ", r: " << rate << std::endl;

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
