#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <mutex>
#include "RiskPortfolio.h"

#include "AmericanTrade.h"
#include "BlackScholesPricer.h"
#include "Bond.h"
#include "EuropeanTrade.h"
#include "JSONReader.h"
#include "Market.h"
#include "Pricer.h"
#include "Swap.h"
#include "Logger.h"
#include "Utils.h"

// using namespace Eigen;

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
    std::cout << "\n============Start of Part 3============" << std::endl;

    Pricer *treePricer = new CRRBinomialTreePricer(700);

    std::vector<double> pricingResults;
    for (auto trade : myPortfolio) {
        double pv = treePricer->Price(mkt, trade);
        pricingResults.push_back(pv);
        std::string tradeInfo = "";
 
        std::cout << "*****Priced trade with PV*****: " << pv << std::endl;
        // log pv details out in a file
        //  Optionally write to a file or store results
        // please output trade info such as id, trade type, notional, start/end/traded price and PV into a txt or csv file
        // logger.info("trade: " + trade->getUUID() + " " + trade->getType() + " " + trade->getUnderlying() + " PV : " + std::to_string(pv));
    	// logger.info("trade: " + trade->toString());
    	if (auto* bond = dynamic_cast<Bond*>(trade)) {
	        tradeInfo = bond->toString();
	        
	    } else if (auto* swap = dynamic_cast<Swap*>(trade)) {
	        tradeInfo = swap->toString();
	        
	    } else if (auto* amerOption = dynamic_cast<AmericanOption*>(trade)) {
	        tradeInfo = amerOption->toString();
	        
	    } else if (auto* euroOption = dynamic_cast<EuropeanOption*>(trade)) {
	        tradeInfo = euroOption->toString();
	    }
	    std::cout << "trade: " << tradeInfo << " " << ", PV: " << pv << std::endl;
	    logger.info("trade: " + tradeInfo + " , PV: " + std::to_string(pv));
    }

    std::cout << "===========end of Part 3============" << std::endl;

    // task 4, analyzing pricing result
    //  a) compare CRR binomial tree result for an european option vs Black
    //  model b) compare CRR binomial tree result for an american option vs
    //  european option

    // task 4, analyzing pricing result
    // a) compare CRR binomial tree result for a European option vs
    // Black-Scholes model results should converge over time
    std::cout << "\n============Start of Part 4============" << std::endl;
    std::cout << "a) Comparing European Option pricing methods" << std::endl;

    for (auto trade : myPortfolio) {
        EuropeanOption *euroOption = dynamic_cast<EuropeanOption *>(trade);
        if (euroOption) {
            std::string opt_type_str = "";
            OptionType opt_type = euroOption->getOptionType();
            if (opt_type == Call){
                opt_type_str = "CALL";
            } else if (opt_type == Put){
                opt_type_str = "PUT";
            }                  
            std::cout<<"\nUnderlying: " << trade->getUnderlying()<<", Instrument : "<< euroOption->getType() <<", Option type : "<<opt_type_str<< ", Strike : " << euroOption->getStrike()<<std::endl; 
            double bsPrice = BlackScholesPricer::Price(mkt, *euroOption);
            double crrPrice = treePricer->Price(mkt, euroOption);
            std::cout << "Comparing European Option: " << std::endl;
            std::cout << "European Option Details: " << euroOption->toString() << std::endl;
            std::cout << "Black-Scholes Price: " << bsPrice << std::endl;
            std::cout << "CRR Binomial Tree Price: " << crrPrice << std::endl;
            logger.info("Comparing European Option BS vs CRR Tree: ");
            logger.info("European Option Details: " + euroOption->toString());
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

    // make use of hashmap to reduce full portfolio O(n^2 comparisons)
    std::cout << "\nb) Comparing pricing results between Amer vs Euro, Call vs Put" << std::endl;

    for (const auto& entry : securityMap) {
	    const SecurityKey& key = entry.first;
	    const std::vector<Trade*>& trades = entry.second;

	    std::vector<AmericanOption*> americanOptions;
	    std::vector<EuropeanOption*> europeanOptions;
                

	    // Separate American and European options
	    for (auto* trade : trades) {
	        if (auto* amerOption = dynamic_cast<AmericanOption*>(trade)) {
	            americanOptions.push_back(amerOption);
	        } else if (auto* euroOption = dynamic_cast<EuropeanOption*>(trade)) {
	            europeanOptions.push_back(euroOption);
	        }
	    }

	    // std::cout << "American options size: " << americanOptions.size() << std::endl;
        // std::cout << "European options size: " << europeanOptions.size() << std::endl;
        std::cout << "\nComparing American and European Options for Type: " << OptionTypeToString(key.optionType)
	                          << ", Strike: " << key.strike << ", Expiry: " << key.expiry << std::endl;
	    // Compare options if both types are present
	    if (!americanOptions.empty() && !europeanOptions.empty()) {
	        for (auto* amerOption : americanOptions) {
	            double amerPrice = treePricer->Price(mkt, amerOption);
                logger.info("Comparing American Option with European Option: ");
	            logger.info("Processing American Option. Underlying= " + amerOption->getUnderlying() +", Type= " + OptionTypeToString(amerOption->getOptionType()) + ", Strike= " + std::to_string(amerOption->getStrike()) +
                    ", Expiry= " + amerOption->GetExpiry().toString()); // !!!
	            for (auto* euroOption : europeanOptions) {
	                double euroPrice = treePricer->Price(mkt, euroOption);
	                logger.info("Processing European Option. Underlying= " + euroOption->getUnderlying() +", Type= " + OptionTypeToString(euroOption->getOptionType()) + ", Strike= " + std::to_string(euroOption->getStrike()) +
                    ", Expiry= " + euroOption->GetExpiry().toString()); // !!!
	                
	                // Log or further process the comparison as needed
	                std::cout << "Comparing American Option with European Option: " << std::endl;
		            std::cout << "*****American Option Price*****: " << amerPrice << std::endl;
		            std::cout << "*****European Option Price*****: " << euroPrice << std::endl;
		            logger.info("*****American Option Price*****: " + std::to_string(amerPrice));
		            logger.info("*****European Option Price*****: " + std::to_string(euroPrice));
	            }
	        }
	    }
	}

    // Just before returning from main, clean up the dynamic memory.
    delete treePricer;
    treePricer = nullptr;

    // final
    std::cout << "\nProject build successfully!" << std::endl;
    logger.info("Ending the application.");


    // Project 2

    // Part 4 a: risk-off
    // testing code
            // Define DV01 and Vega coefficients for each trade
    // Example data
    Eigen::VectorXd dv01(3);
    dv01 << 1000, -500, 200;

    Eigen::VectorXd vega(3);
    vega << 50, 70, -30;

    Eigen::VectorXd price(3);
    price << 100, 200, 150;

    double capitalLimit = 100000;

    // Create an instance of the RiskPortfolio class
    RiskPortfolio riskPortfolio(dv01, vega, price, capitalLimit);

    // Compute the optimal weights
    Eigen::VectorXd weights = riskPortfolio.createRiskPortfolio();

    // Output the results
    std::cout << "Optimized weights to neutralize DV01 and Vega within capital limit:" << std::endl;
    for (int i = 0; i < weights.size(); ++i) {
        std::cout << "Weight for Trade " << i + 1 << ": " << weights[i] << std::endl;
    }


    return 0;
}
