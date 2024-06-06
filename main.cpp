#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <mutex>

#include "AmericanTrade.h"
#include "BlackScholesPricer.h"
#include "Bond.h"
#include "EuropeanTrade.h"
#include "JSONReader.h"
#include "Market.h"
#include "Pricer.h"
#include "Swap.h"
#include "Logger.h"

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
    localtime_r(&in_time_t, &bt);  // POSIX
#elif defined(_MSC_VER)
    localtime_s(&bt, &in_time_t);  // Windows
#else
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    bt = *std::localtime(&in_time_t);  // Not thread-safe as a last resort
#endif

    // Format the time string to 'YYYYMMDD_HHMMSS'
    std::ostringstream oss;
    oss << std::put_time(&bt, "output_%Y%m%d_%H%M%S.txt");
    return oss.str();
}

// we want to compare options between american and european options
// but with the same type (call/put), strike and expiry.
struct SecurityKey {
    OptionType optionType;  // Type of the option (call or put)
    double strike;
    Date expiry;

    bool operator==(const SecurityKey& other) const {
        return std::tie(optionType, strike, expiry) == std::tie(other.optionType, other.strike, other.expiry);
    }
};

struct SecurityHash {
    size_t operator()(const SecurityKey& k) const {
        // Combine hashes of all key components
        size_t hash_optionType = std::hash<int>()(static_cast<int>(k.optionType));        
        size_t hash_strike = std::hash<double>()(k.strike);
        // size_t hash_expiry = std::hash<int>()(k.expiry.year * 10000 + k.expiry.month * 100 + k.expiry.day);
        size_t hash_expiry = std::hash<std::string>()(k.expiry.toString());

        // A robust way to combine hashes
        return hash_optionType ^ (hash_strike << 1) ^ (hash_expiry << 2); // Left shift each hash by a different amount to reduce hash collision
    }
};

std::map<OptionType, std::string> optionTypeNames = {
    {Call, "Call"},
    {Put, "Put"},
    {BinaryCall, "BinaryCall"},
    {BinaryPut, "BinaryPut"}
};

std::string OptionTypeToString(OptionType type) {
    auto it = optionTypeNames.find(type);
    if (it != optionTypeNames.end()) {
        return it->second;
    }
    return "Unknown";
}

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
    std::unordered_map<SecurityKey, std::vector<Trade*>, SecurityHash> securityMap;
	
	for (auto& trade : myPortfolio) {
	    // Check if the trade is an AmericanOption
	    if (auto amerOption = dynamic_cast<AmericanOption*>(trade)) {
	        SecurityKey key{amerOption->getOptionType(), amerOption->getStrike(), amerOption->GetExpiry()};
	        std::cout << "Inserting amer option: " << key.optionType << ", " << key.strike << ", " << key.expiry << std::endl;
	        securityMap[key].push_back(amerOption);
	    }
	    // Check if the trade is a EuropeanOption
	    else if (auto euroOption = dynamic_cast<EuropeanOption*>(trade)) {
	        SecurityKey key{euroOption->getOptionType(), euroOption->getStrike(), euroOption->GetExpiry()};
	        std::cout << "Inserting euro option: " << key.optionType << ", " << key.strike << ", " << key.expiry << std::endl;
	        securityMap[key].push_back(euroOption);
	    }
	}

    // task 3, create a pricer and price the portfolio, output the pricing
    // result of each deal.
	std::filesystem::path OUTPUT_PATH =
        std::filesystem::current_path() / "../../output";
    std::string output_filename = generateDateTimeFilename();
    Logger logger((OUTPUT_PATH / output_filename).string());  // Initialize the logger
    // Example of using the logger
    logger.info("Starting the application.");
    // Log data path
    logger.info("Ouput path: " + OUTPUT_PATH.string());
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
        logger.info("trade: " + trade->getType() + " " + trade->getUnderlying() + " PV : " + std::to_string(pv));
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
            logger.info("Comparing European Option: ");
            logger.info("Black-Scholes Price: " + std::to_string(bsPrice));
            logger.info("CRR Binomial Tree Price: " + std::to_string(crrPrice));
        }
    }

    // b) compare CRR binomial tree result for an American option vs European
    // option compare between US call / put vs EU call / put
    // only compare between american and european options with same:
    // (1) option type ( call / put )
    // (2) strike price
    // (3) expiration date
    for (const auto& entry : securityMap) {
	    const SecurityKey& key = entry.first;
	    const std::vector<Trade*>& trades = entry.second;

	    std::vector<AmericanOption*> americanOptions;
	    std::vector<EuropeanOption*> europeanOptions;

	    std::cout << "Key - OptionType: " << key.optionType << " Strike: " << key.strike << ", Expiry: " << key.expiry.toString() << std::endl;
        

	    // Separate American and European options
	    for (auto* trade : trades) {
	        if (auto* amerOption = dynamic_cast<AmericanOption*>(trade)) {
	            americanOptions.push_back(amerOption);
	        } else if (auto* euroOption = dynamic_cast<EuropeanOption*>(trade)) {
	            europeanOptions.push_back(euroOption);
	        }
	    }

	    std::cout << "American options size: " << americanOptions.size() << std::endl;
        std::cout << "European options size: " << europeanOptions.size() << std::endl;

	    // Compare options if both types are present
	    if (!americanOptions.empty() && !europeanOptions.empty()) {
	        for (auto* amerOption : americanOptions) {
	            double amerPrice = treePricer->Price(mkt, amerOption);
	            logger.info("Processing American Option. Underlying= " + amerOption->getUnderlying() +", Type= " + OptionTypeToString(amerOption->getOptionType()) + ", Strike= " + std::to_string(amerOption->getStrike()) +
                    ", Expiry= " + amerOption->GetExpiry().toString()); // !!!
	            for (auto* euroOption : europeanOptions) {
	                double euroPrice = treePricer->Price(mkt, euroOption);
	                logger.info("Processing European Option. Underlying= " + euroOption->getUnderlying() +", Type= " + OptionTypeToString(euroOption->getOptionType()) + ", Strike= " + std::to_string(euroOption->getStrike()) +
                    ", Expiry= " + euroOption->GetExpiry().toString()); // !!!
	                std::cout << "Comparing American and European Options for Type: " << OptionTypeToString(key.optionType)
	                          << ", Strike: " << key.strike << ", Expiry: " << key.expiry << std::endl;
	                // Log or further process the comparison as needed
	                std::cout << "Comparing American Option with European Option: " << std::endl;
		            std::cout << "*****American Option Price*****: " << amerPrice << std::endl;
		            std::cout << "*****European Option Price*****: " << euroPrice << std::endl;
		            logger.info("Comparing American Option with European Option: ");
		            logger.info("*****American Option Price*****: " + std::to_string(amerPrice));
		            logger.info("*****European Option Price*****: " + std::to_string(euroPrice));
	            }
	        }
	    }
	}

    // final
    cout << "Project build successfully!" << endl;
    return 0;
}
