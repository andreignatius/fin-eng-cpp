#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <unistd.h>

#include "AmericanTrade.h"
#include "Bond.h"
#include "EuropeanTrade.h"
#include "Market.h"
#include "Pricer.h"
#include "Swap.h"
#include "BlackScholesPricer.h"

using namespace std;

void readFromFile(const string &fileName, string &outPut) {
    string lineText;
    ifstream MyReadFile(fileName);
    while (getline(MyReadFile, lineText)) {
        outPut.append(lineText);
    }
    MyReadFile.close();
}

int main() {
    // task 1, create an market data object, and update the market data from
    // from txt file
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
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::cout << "Current working dir: " << cwd << std::endl;
    } else {
        perror("getcwd() error");
    }
    mkt.updateMarketFromVolFile("../../vol_bond.csv", "BondTrade"); // Update market data from file
    // mkt.updateMarketFromVolFile("../../vol_swap.csv", "SwapTrade"); // Update market data from file
    // mkt.updateMarketFromVolFile("../../vol_amer.csv", "TreeProduct"); // Update market data from file
    
    // mkt.updateMarketFromVolFile("../../vol_euro.csv", "euro"); // Update market data from file
    // mkt.updateMarketFromVolFile("../../voldummycurve.csv", "vol");
    
    // mkt.updateMarketFromVolFile("../../vol.txt", "vol");

    mkt.updateMarketFromStockFile("../../stockPrice.txt");  // Load stock prices
    mkt.updateMarketFromCurveFile("../../curve.txt", "USD-SOFR");
    mkt.Print();          // Check loaded data

    // TODO : create more bonds / swaps/ european option / american options
    // task 2, create a portfolio of bond, swap, european option, american
    // option for each time, at least should have long / short, different tenor
    // or expiry, different underlying totally no less than 16 trades
    vector<Trade *> myPortfolio;

    // Adding Bonds
    myPortfolio.push_back(new Bond(Date(2024, 1, 1), Date(2034, 1, 1), 10000000,
                                   103.5)); // Long position
    myPortfolio.push_back(new Bond(Date(2024, 1, 1), Date(2029, 1, 1), 5000000,
                                   105.0)); // Short position
    // myPortfolio.push_back(new Bond(Date(2024, 1, 1), Date(2025, 1, 1), 10000000,
    //                                101.5)); // Long position
    // myPortfolio.push_back(new Bond(Date(2024, 1, 1), Date(2026, 1, 1), 5000000,
    //                                102.0)); // Short position

    // Adding Swaps
    myPortfolio.push_back(new Swap(Date(2024, 5, 17), Date(2029, 1, 1), 2000000,
                                   0.01, 1, true,
                                   mkt)); // Fixed-for-floating, annual
    myPortfolio.push_back(new Swap(Date(2024, 1, 1), Date(2029, 1, 1), 2000000,
                                   0.02, 2, false,
                                   mkt)); // Floating-for-fixed, semi-annual
    // myPortfolio.push_back(new Swap(Date(2024, 1, 1), Date(2026, 1, 1), 2000000,
    //                                0.01, 1, true,
    //                                mkt)); // Fixed-for-floating, annual
    // myPortfolio.push_back(new Swap(Date(2024, 1, 1), Date(2026, 1, 1), 2000000,
    //                                0.02, 2, false,
    //                                mkt)); // Floating-for-fixed, semi-annual

    // Adding European Options
    myPortfolio.push_back(
        new EuropeanOption(Call, 100, Date(2025, 12, 31))); // Call option
    myPortfolio.push_back(
        new EuropeanOption(Put, 100, Date(2025, 12, 31))); // Put option
    // myPortfolio.push_back(
    //     new EuropeanOption(Call, 100, Date(2024, 12, 31))); // Call option
    // myPortfolio.push_back(
    //     new EuropeanOption(Put, 100, Date(2024, 12, 31))); // Put option

    // Adding American Options
    myPortfolio.push_back(
        new AmericanOption(Call, 100, Date(2025, 12, 31))); // Call option
    myPortfolio.push_back(
        new AmericanOption(Put, 100, Date(2025, 12, 31))); // Put option
    // myPortfolio.push_back(
    //     new AmericanOption(Call, 100, Date(2024, 12, 31))); // Call option
    // myPortfolio.push_back(
    //     new AmericanOption(Put, 100, Date(2024, 12, 31))); // Put option

    // task 3, create a pricer and price the portfolio, output the pricing result
    // of each deal.
    Pricer *treePricer = new CRRBinomialTreePricer(10);
    std::vector<double> pricingResults;
    for (auto trade : myPortfolio) {
        double pv = treePricer->Price(mkt, trade);
        pricingResults.push_back(pv);
        std::cout << "Priced trade with PV: " << pv << std::endl;
        // log pv details out in a file
        //  Optionally write to a file or store results
    }

    // Pricer* pricer = new Pricer();
    // std::vector<double> pricingResults;
    // for (auto trade: myPortfolio){
    //     double pv = pricer->Price(mkt, trade);
    //     pricingResults.push_back(pv);
    //     std::cout<< "Priced trade with PV: "<<pv<<std::endl;

    // }

    // task 4, analyzing pricing result
    //  a) compare CRR binomial tree result for an european option vs Black
    //  model b) compare CRR binomial tree result for an american option vs
    //  european option

    // task 4, analyzing pricing result
    // a) compare CRR binomial tree result for a European option vs Black-Scholes model
    // results should converge over time
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

    // b) compare CRR binomial tree result for an American option vs European option
    // compare between US call / put vs EU call / put
    for (auto trade : myPortfolio) {
        AmericanOption *amerOption = dynamic_cast<AmericanOption *>(trade);
        if (amerOption) {
            for (auto trade2 : myPortfolio) {
                EuropeanOption *euroOption = dynamic_cast<EuropeanOption *>(trade2);
                if (euroOption && euroOption->getStrike() == amerOption->getStrike() &&
                    euroOption->GetExpiry() == amerOption->GetExpiry() && euroOption->getOptionType() == amerOption->getOptionType()) {
                    double amerPrice = treePricer->Price(mkt, amerOption);
                    double euroPrice = treePricer->Price(mkt, euroOption);
                    std::cout << "Comparing American Option with European Option: " << std::endl;
                    std::cout << "American Option Price: " << amerPrice << std::endl;
                    std::cout << "European Option Price: " << euroPrice << std::endl;
                }
            }
        }
    }

    // final
    cout << "Project build successfully!" << endl;
    return 0;
}
