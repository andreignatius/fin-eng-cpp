#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "AmericanTrade.h"
#include "BlackScholesPricer.h"
#include "Bond.h"
#include "EuropeanTrade.h"
#include "JSONReader.h"
#include "Market.h"
#include "Pricer.h"
#include "Swap.h"

using namespace std;

/*
Comments: when using new, pls remember to use delete for ptr
*/

int main() {
    // task 1, create an market data object, and update the market data from
    // from txt file
    std::filesystem::path DATA_PATH =
        std::filesystem::current_path() / "../../data";
    std::time_t t = std::time(0);
    auto now_ = std::localtime(&t);
    Date valueDate;
    valueDate.year = now_->tm_year + 1900;
    valueDate.month = now_->tm_mon + 1;
    valueDate.year = now_->tm_mday;

    Market mkt = Market(valueDate);
    /*
    load data from file and update market object with data
    */

    std::cout << DATA_PATH / "vol_bond.csv111" << std::endl;
    mkt.updateMarketFromVolFile((DATA_PATH / "vol_bond.csv").string(),
                                "BondTrade"); // Update market data from file
    mkt.updateMarketFromVolFile((DATA_PATH / "vol_swap.csv").string(),
                                "SwapTrade"); // Update market data from file
    mkt.updateMarketFromVolFile(
        (DATA_PATH / "vol_amer.csv").string(),
        "AmericanOption"); // Update market data from file
    mkt.updateMarketFromVolFile(
        (DATA_PATH / "vol_euro.csv").string(),
        "EuropeanOption"); // Update market data from file
    // mkt.updateMarketFromVolFile("../../voldummycurve.csv", "vol");

    // mkt.updateMarketFromVolFile("../../data/vol.txt", "vol");

    mkt.updateMarketFromBondFile(
        (DATA_PATH / "bondPrice.txt").string()); // Load bond prices

    mkt.updateMarketFromStockFile(
        (DATA_PATH / "stockPrice.csv").string()); // Load stock prices
    // mkt.updateMarketFromCurveFile("../../data/curve.txt", "USD-SOFR");
    mkt.updateMarketFromCurveFile((DATA_PATH / "sofrdummycurve.csv").string(),
                                  "USD-SOFR");
    mkt.Print(); // Check loaded data

    // TODO : create more bonds / swaps/ european option / american options
    // task 2, create a portfolio of bond, swap, european option, american
    // option for each time, at least should have long / short, different tenor
    // or expiry, different underlying totally no less than 16 trades

    /*
        JOS 2024/05/25
            This utilizes the JSONReader functionality,
            Users supply a JSON file containing the portfolio.
            JSONReader will parse and construct the portfolio vector.
    */
    vector<Trade *> myPortfolio;
    JSONReader myJSONReader((DATA_PATH / "portfolio.json").string(), mkt,
                            myPortfolio);
    std::cout << "porfolio from JSON" << std::endl;
    myJSONReader.constructPortfolio();
    myJSONReader.getMarketObject().Print();

    // why do i need to re-set myPortfolio?
    myPortfolio = myJSONReader.getPortfolio();

    std::cout << "============Start of Part 3============" << std::endl;

    Pricer *treePricer = new CRRBinomialTreePricer(100);
    std::vector<double> pricingResults;
    for (auto trade : myPortfolio) {
        std::cout << "trade: " << trade->getType() << ", underlying: " << trade->getUnderlying() << std::endl;
        double pv = treePricer->Price(mkt, trade);
        pricingResults.push_back(pv);
        std::cout << "trade: " << trade->getType() << " "
                  << trade->getUnderlying() << std::endl;
        std::cout << "*****Priced trade with PV*****: " << pv << std::endl;
        // log pv details out in a file
        //  Optionally write to a file or store results

        // please output trade info such as id, trade type, notional, start/end/traded price and PV into a txt or csv file
    }

    std::cout << "===========end of Part 3============" << std::endl;

    // task 4, analyzing pricing result
    //  a) compare CRR binomial tree result for an european option vs Black
    //  model b) compare CRR binomial tree result for an american option vs
    //  european option

    // task 4, analyzing pricing result
    // a) compare CRR binomial tree result for a European option vs
    // Black-Scholes model results should converge over time
    for (auto trade : myPortfolio) {
        EuropeanOption *euroOption = dynamic_cast<EuropeanOption *>(trade);
        if (euroOption) {
            double bsPrice = BlackScholesPricer::Price(mkt, *euroOption);
            double crrPrice = treePricer->Price(mkt, euroOption);
            std::cout << "Comparing European Option: " << std::endl;
            std::cout << "Black-Scholes Price: " << bsPrice << std::endl;
            std::cout << "CRR Binomial Tree Price: " << crrPrice << std::endl;
        }
    }

    // b) compare CRR binomial tree result for an American option vs European
    // option compare between US call / put vs EU call / put
    for (auto trade : myPortfolio) {
        AmericanOption *amerOption = dynamic_cast<AmericanOption *>(trade);
        if (amerOption) {
            for (auto trade2 : myPortfolio) {
                EuropeanOption *euroOption =
                    dynamic_cast<EuropeanOption *>(trade2);
                if (euroOption &&
                    euroOption->getStrike() == amerOption->getStrike() &&
                    euroOption->GetExpiry() == amerOption->GetExpiry() &&
                    euroOption->getOptionType() ==
                        amerOption->getOptionType()) {
                    double amerPrice = treePricer->Price(mkt, amerOption);
                    double euroPrice = treePricer->Price(mkt, euroOption);
                    std::cout
                        << "Comparing American Option with European Option: "
                        << std::endl;
                    std::cout
                        << "*****American Option Price*****: " << amerPrice
                        << std::endl;
                    std::cout
                        << "*****European Option Price*****: " << euroPrice
                        << std::endl;
                }
            }
        }
    }

    // final
    cout << "Project build successfully!" << endl;
    return 0;
}
