#include "AmericanTrade.h"
#include "BlackScholesPricer.h"
#include "Bond.h"
#include "EuropeanTrade.h"
#include "JSONReader.h"
#include "Logger.h"
#include "Market.h"
#include "PortfolioMaker.h"
#include "Pricer.h"
#include "RiskEngine.h"
#include "Swap.h"
#include "Utils.h"
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <string>

#define USE_MULTITHREADING 0

/*
Comments: when using new, pls remember to use delete for ptr
*/

std::string generateDateTimeFilename() {
    // Get current time point
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    // Convert to local time
    std::tm bt;
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    localtime_r(&in_time_t, &bt); // POSIX
#elif defined(_MSC_VER)
    localtime_s(&bt, &in_time_t); // Windows
#else
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    bt = *std::localtime(&in_time_t); // Not thread-safe as a last resort
#endif

    // Format the time string to 'YYYYMMDD_HHMMSS'
    std::ostringstream oss;
    oss << std::put_time(&bt, "output_%Y%m%d_%H%M%S.txt");
    return oss.str();
}

// we want to compare options between american and european options
// but with the same type (call/put), strike and expiry.
struct SecurityKey {
    OptionType optionType; // Type of the option (call or put)
    double strike;
    Date expiry;

    bool operator==(const SecurityKey &other) const {
        return std::tie(optionType, strike, expiry) ==
               std::tie(other.optionType, other.strike, other.expiry);
    }
};

struct SecurityHash {
    size_t operator()(const SecurityKey &k) const {
        // Combine hashes of all key components
        size_t hash_optionType =
            std::hash<int>()(static_cast<int>(k.optionType));
        size_t hash_strike = std::hash<double>()(k.strike);
        // size_t hash_expiry = std::hash<int>()(k.expiry.year * 10000 +
        // k.expiry.month * 100 + k.expiry.day);
        size_t hash_expiry = std::hash<std::string>()(k.expiry.toString());

        // A robust way to combine hashes
        return hash_optionType ^ (hash_strike << 1) ^
               (hash_expiry << 2); // Left shift each hash by a different amount
                                   // to reduce hash collision
    }
};

int main() {
    // task 1, create an market data object, and update the market data from
    // from txt file
    std::filesystem::path DATA_PATH =
        std::filesystem::current_path() / "../../data";
    std::filesystem::path MKT_DATA_PATH =
        std::filesystem::current_path() / "../../market_data";
    std::time_t t = std::time(0);
    auto now_ = std::localtime(&t);
    /*
        USER SHOULD CHANGE THIS , EITHER 2024-06-01 OR 2024-06-02
    */
    Date valueDate = Date(2024, 6, 2);
    std::cout << "WE ARE AT VALUE DATE : " << valueDate << std::endl;
    Market mkt = Market(valueDate);
    /*
    load data from file and update market object with data
    */

    mkt.updateMarketFromCurveFile(
        (MKT_DATA_PATH / "usd_sofr_20240601.csv").string(), "usd-sofr",
        Date(2024, 6, 1));
    mkt.updateMarketFromCurveFile(
        (MKT_DATA_PATH / "usd_sofr_20240602.csv").string(), "usd-sofr",
        Date(2024, 6, 2));
    mkt.updateMarketFromCurveFile(
        (MKT_DATA_PATH / "usd_sofr_20240601.csv").string(), "usd-gov",
        Date(2024, 6, 1));
    mkt.updateMarketFromCurveFile(
        (MKT_DATA_PATH / "usd_sofr_20240602.csv").string(), "usd-gov",
        Date(2024, 6, 2));

    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240601.csv").string(),
                                "BondTrade", Date(2024, 6, 1));
    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240602.csv").string(),
                                "BondTrade", Date(2024, 6, 2));

    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240601.csv").string(),
                                "SwapTrade", Date(2024, 6, 1));
    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240602.csv").string(),
                                "SwapTrade", Date(2024, 6, 2));

    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240601.csv").string(),
                                "AAPL", Date(2024, 6, 1));
    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240602.csv").string(),
                                "AAPL", Date(2024, 6, 2));

    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240601.csv").string(),
                                "SP500", Date(2024, 6, 1));
    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240602.csv").string(),
                                "SP500", Date(2024, 6, 2));

    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240601.csv").string(),
                                "EuropeanOption", Date(2024, 6, 1));
    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240602.csv").string(),
                                "EuropeanOption", Date(2024, 6, 2));

    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240601.csv").string(),
                                "STI", Date(2024, 6, 1));
    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240602.csv").string(),
                                "STI", Date(2024, 6, 2));

    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240601.csv").string(),
                                "usd-sofr", Date(2024, 6, 1));
    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240602.csv").string(),
                                "usd-sofr", Date(2024, 6, 2));

    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240601.csv").string(),
                                "AmericanOption", Date(2024, 6, 1));
    mkt.updateMarketFromVolFile((MKT_DATA_PATH / "vol_20240602.csv").string(),
                                "AmericanOption", Date(2024, 6, 2));

    mkt.updateMarketFromBondFile(
        (DATA_PATH / "bondPrice.txt").string()); // Load bond prices

    mkt.updateMarketFromStockFile(
        (MKT_DATA_PATH / "stock_price_20240601.csv").string(),
        Date(2024, 6, 1)); // Load stock prices
    mkt.updateMarketFromStockFile(
        (MKT_DATA_PATH / "stock_price_20240602.csv").string(),
        Date(2024, 6, 2)); // Load stock prices

    /*
    1b. Portfolio from CSV
    */
    CSVReader myCSVReader =
        CSVReader((MKT_DATA_PATH / "portfolio.csv").string());
    std::unordered_map<std::string, std::vector<std::string>> myMap;
    myMap = myCSVReader.parseFile();
    vector<std::unique_ptr<Trade>> myPortfolio;
    myPortfolio = PortfolioMaker::constructPortfolio(valueDate, myMap, mkt);
    // JSONReader myJSONReader((MKT_DATA_PATH / "portfolio.json").string(), mkt,
    //                         myPortfolio, Date(2024, 6, 1));
    // myJSONReader.constructPortfolio();
    // myJSONReader.getMarketObject().Print();

    // JSONReader myJSONReaderDay2((MKT_DATA_PATH / "portfolio.json").string(),
    // mkt,
    //                         myPortfolio, Date(2024, 6, 2));
    // myJSONReaderDay2.constructPortfolio();
    // myJSONReaderDay2.getMarketObject().Print();

    // Create and construct portfolio for day 1
    /*
    JSONReader myJSONReader((MKT_DATA_PATH / "portfolio.json").string(), mkt,
                            myPortfolio, Date(2024, 6, 1));
    myJSONReader.constructPortfolio();
    myJSONReader.getMarketObject().Print();

    // Extend the existing portfolio with day 1 trades
    vector<std::unique_ptr<Trade>> &tempPortfolio1 =
        myJSONReader.getPortfolio();
    myPortfolio.insert(myPortfolio.end(),
                       std::make_move_iterator(tempPortfolio1.begin()),
                       std::make_move_iterator(tempPortfolio1.end()));
    tempPortfolio1
        .clear(); // Clear the temporary portfolio to ensure it's empty

    // Create and construct portfolio for day 2
    JSONReader myJSONReaderDay2((MKT_DATA_PATH / "portfolio.json").string(),
                                mkt, myPortfolio, Date(2024, 6, 2));
    myJSONReaderDay2.constructPortfolio();
    myJSONReaderDay2.getMarketObject().Print();

    // Extend the existing portfolio with day 2 trades
    vector<std::unique_ptr<Trade>> &tempPortfolio2 =
        myJSONReaderDay2.getPortfolio();
    myPortfolio.insert(myPortfolio.end(),
                       std::make_move_iterator(tempPortfolio2.begin()),
                       std::make_move_iterator(tempPortfolio2.end()));
    tempPortfolio2.clear(); // Clear the temporary portfolio
    */
    // Optionally print or validate the combined portfolio
    mkt.Print();
    std::cout << "Combined portfolio size: " << myPortfolio.size() << std::endl;

    // why do i need to re-set myPortfolio?
    // Move the portfolio
    // myPortfolio = std::move(myJSONReader.getPortfolio());

    std::unordered_map<SecurityKey, std::vector<Trade *>, SecurityHash>
        securityMap;

    for (auto &trade : myPortfolio) {
        // Check if the trade is an AmericanOption
        if (auto amerOption = dynamic_cast<AmericanOption *>(trade.get())) {
            SecurityKey key{amerOption->getOptionType(),
                            amerOption->getStrike(), amerOption->GetExpiry()};
            std::cout << "Inserting amer option: " << key.optionType << ", "
                      << key.strike << ", " << key.expiry << std::endl;
            securityMap[key].push_back(amerOption);
        }
        // Check if the trade is a EuropeanOption
        else if (auto euroOption =
                     dynamic_cast<EuropeanOption *>(trade.get())) {
            SecurityKey key{euroOption->getOptionType(),
                            euroOption->getStrike(), euroOption->GetExpiry()};
            std::cout << "Inserting euro option: " << key.optionType << ", "
                      << key.strike << ", " << key.expiry << std::endl;
            securityMap[key].push_back(euroOption);
        }
    }

    // task 3, create a pricer and price the portfolio, output the pricing
    // result of each deal.
    std::filesystem::path OUTPUT_PATH =
        std::filesystem::current_path() / "../../output";
    std::string output_filename = generateDateTimeFilename();
    Logger logger(
        (OUTPUT_PATH / output_filename).string()); // Initialize the logger
    // Example of using the logger
    logger.info("Starting the application.");
    // Log data path
    logger.info("Ouput path: " + OUTPUT_PATH.string());
    std::cout << "\n============Start of Part 3============" << std::endl;

    std::unique_ptr<Pricer> treePricer =
        std::make_unique<CRRBinomialTreePricer>(700);

    std::vector<double> pricingResults;

    auto start = std::chrono::high_resolution_clock::now();

    RiskEngine myRiskEngine = RiskEngine(mkt);

#if USE_MULTITHREADING
    std::vector<std::future<void>>
        futures; // To store futures of asynchronous tasks

    for (auto &trade : myPortfolio) {
        // Launch asynchronous tasks for each trade
        std::cout << "***** Start PV Pricing and Risk Test *****" << std::endl;
        double pv = treePricer->Price(mkt, trade.get(), Date(2024, 6, 1));
        std::cout << "+++" << std::endl;
        std::cout << "***** Priced trade with PV *****: " << pv << std::endl;

        std::vector<double> dv01Results;
		std::vector<double> vegaResults;
        // Async DV01 calculation
        auto dv01Future = std::async(
            std::launch::async, [&myRiskEngine, &trade, &mkt, &treePricer]() {
                std::cout << "====================== DV01 CALCULATION "
                             "======================"
                          << std::endl;
                myRiskEngine.computeRisk("dv01", trade.get(), Date(2024, 6, 1),
                                         treePricer.get(), dv01Results, vegaResults);
            });

        // Async VEGA calculation
        auto vegaFuture = std::async(
            std::launch::async, [&myRiskEngine, &trade, &mkt, &treePricer]() {
                std::cout << "====================== VEGA CALCULATION "
                             "======================"
                          << std::endl;
                myRiskEngine.computeRisk("vega", trade.get(), Date(2024, 6, 1),
                                         treePricer.get(), dv01Results, vegaResults);
            });

        futures.push_back(std::move(dv01Future));
        futures.push_back(std::move(vegaFuture));
        pricingResults.push_back(
            pv); // Assuming pricingResults is defined elsewhere
    }

    // Wait for all futures to complete
    for (auto &future : futures) {
        future.get(); // This blocks until the task completes
    }
#else
    std::cout << "RE-CHECK VALUEDATE : " << valueDate << std::endl;
    for (auto &trade : myPortfolio) {
        std::cout << "***** Start PV Pricing and Risk Test *****" << std::endl;
        std::cout << "====================== PV PRICING ======================"
                  << std::endl;
        std::vector<double> dv01Results;
		std::vector<double> vegaResults;
        double pv = treePricer->Price(
            mkt, trade.get(),
            valueDate); // Assuming Price() accepts a raw pointer
        std::cout << "Priced trade with PV : " << pv << std::endl;
        double dv01 = 0; //     treePricer->CalculateDV01(mkt, trade.get(),
                         //     Date(2024, 6, 1));
        double vega = 0;
        std::cout
            << "====================== DV01 CALCULATION ======================"
            << std::endl;
        myRiskEngine.computeRisk("dv01", trade.get(), valueDate,
                                 treePricer.get(), dv01Results, vegaResults);
        std::cout
            << "====================== VEGA CALCULATION ======================"
            << std::endl;
        myRiskEngine.computeRisk("vega", trade.get(), valueDate,
                                 treePricer.get(), dv01Results, vegaResults);
        pricingResults.push_back(pv);
        std::string tradeInfo = "";

        std::cout << "for trade : " << trade->getUUID() << " , " << trade->getUnderlying() << std::endl;
        for (auto& dv01 : dv01Results) {
        	std::cout << "dv01 : " << dv01 << std::endl;
        }
        std::cout << "===========================" << std::endl;
        for (auto& vega : vegaResults) {
        	std::cout << "vega : " << vega << std::endl;
        }
        std::cout << "~~~~~ End of PV Pricing and Risk Test ~~~~~" << std::endl;
    }
#endif
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " ms\n";
    std::cout << "========================================================="
              << std::endl;

    std::cout << "===========end of Part 3============" << std::endl;

    // final
    std::cout << "\nProject build successfully!" << std::endl;
    // logger.info("Ending the application.");

    return 0;
}
