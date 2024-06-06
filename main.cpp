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

using namespace std;

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

// we want to compare options across types but with the same strike and expiry.
// struct SecurityKey {
//     double strike;
//     Date expiry;

//     bool operator==(const SecurityKey& other) const {
//         return std::tie(strike, expiry) == std::tie(other.strike, other.expiry);
//     }
// };

// struct SecurityHash {
//     size_t operator()(const SecurityKey& k) const {
//         std::size_t h1 = std::hash<double>()(k.strike);
//         std::size_t h2 = std::hash<int>()(k.expiry.year * 10000 + k.expiry.month * 100 + k.expiry.day);
//         return h1 ^ (h2 << 1);  // Shift h2 left to avoid collisions
//     }
// };


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

// struct SecurityHash {
//     size_t operator()(const SecurityKey& k) const {
//         size_t h1 = std::hash<int>()(static_cast<int>(k.optionType));
//         size_t h2 = std::hash<double>()(k.strike);
//         size_t h3 = std::hash<int>()(k.expiry.year * 10000 + k.expiry.month * 100 + k.expiry.day);

//         size_t hash = 17;
//         hash = hash * 31 + h1;
//         hash = hash * 31 + h2;
//         hash = hash * 31 + h3;

//         return hash;
//     }
// };


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
    // std::unordered_map<SecurityKey, std::pair<std::vector<AmericanOption*>, std::vector<EuropeanOption*>>, SecurityHash> securityMap;
    // Populate the map
	// for (auto& trade : myPortfolio) {
	//     SecurityKey key{trade->getType(), trade->getStrike(), trade->GetExpiry()};
	//     securityMap[key].push_back(trade);
	// }
	
	for (auto& trade : myPortfolio) {

	    // Check if the trade is an AmericanOption
	    if (auto amerOption = dynamic_cast<AmericanOption*>(trade)) {
	        SecurityKey key{amerOption->getOptionType(), amerOption->getStrike(), amerOption->GetExpiry()};
	        std::cout << "Inserting0: " << key.optionType << ", " << key.strike << ", " << key.expiry << std::endl;
    
	        securityMap[key].push_back(amerOption);
	    }
	    // Check if the trade is a EuropeanOption
	    else if (auto euroOption = dynamic_cast<EuropeanOption*>(trade)) {
	        SecurityKey key{euroOption->getOptionType(), euroOption->getStrike(), euroOption->GetExpiry()};
	        std::cout << "Inserting1: " << key.optionType << ", " << key.strike << ", " << key.expiry << std::endl;
    
	        securityMap[key].push_back(euroOption);
	    }
	}

	SecurityKey key1{OptionType::Call, 700, Date(2025, 12, 31)};
	SecurityKey key2{OptionType::Call, 700, Date(2025, 12, 31)};

	SecurityHash hasher;
	auto hash1 = hasher(key1);
	auto hash2 = hasher(key2);

	std::cout << "Hash1: " << hash1 << ", Hash2: " << hash2 << std::endl;

	if (key1.expiry == key2.expiry) {
	    std::cout << "Dates are equal." << std::endl;
	} else {
	    std::cout << "Dates are not equal." << std::endl;
	}
	// std::cout << "key1 expiry: " << key1.expiry.year << " " << key1.expiry.month << " " << key1.expiry.day << std::endl;
	// std::cout << "key2 expiry: " << key2.expiry.year << " " << key2.expiry.month << " " << key2.expiry.day << std::endl;
	std::cout << "key1 expiry: " << key1.expiry.toString() << std::endl;
	std::cout << "key2 expiry: " << key2.expiry.toString() << std::endl;

	size_t hash_optionType = std::hash<int>()(static_cast<int>(key1.optionType));        
    size_t hash_strike = std::hash<double>()(key1.strike);
    size_t hash_expiry = std::hash<int>()(key1.expiry.year * 10000 + key1.expiry.month * 100 + key1.expiry.day);
    std::cout << "hash_optionType1: " << hash_optionType << std::endl;
    std::cout << "hash_strike1: " << hash_strike << std::endl;
    std::cout << "hash_expiry1: " << hash_expiry << std::endl;

    size_t hash_optionType2 = std::hash<int>()(static_cast<int>(key2.optionType));        
    size_t hash_strike2 = std::hash<double>()(key2.strike);
    size_t hash_expiry2 = std::hash<int>()(key2.expiry.year * 10000 + key2.expiry.month * 100 + key2.expiry.day);
    std::cout << "hash_optionType2: " << hash_optionType2 << std::endl;
    std::cout << "hash_strike2: " << hash_strike2 << std::endl;
    std::cout << "hash_expiry2: " << hash_expiry2 << std::endl;
	// for (auto& trade : myPortfolio) {
	//     if (auto amerOption = dynamic_cast<AmericanOption*>(trade)) {
	//         SecurityKey key{amerOption->getOptionType(), amerOption->getStrike(), amerOption->GetExpiry()};
	//         securityMap[key].first.push_back(amerOption);
	//     }
	//     if (auto euroOption = dynamic_cast<EuropeanOption*>(trade)) {
	//         SecurityKey key{euroOption->getOptionType(), euroOption->getStrike(), euroOption->GetExpiry()};
	//         securityMap[key].second.push_back(euroOption);
	//     }
	// }

    // task 3, create a pricer and price the portfolio, output the pricing
    // result of each deal.

    // // Adding Bonds
    // myPortfolio.push_back(new Bond(Date(2024, 1, 1), Date(2034, 1, 1), 10000000,
    //                                103.5, 0.025, "SGD-GOV") ); // Long position
    // myPortfolio.push_back(new Bond(Date(2024, 1, 1), Date(2029, 1, 1), 5000000,
    //                                105.0, 0.025, "SGD-GOV") ); // Short position

    // // Adding Swaps
    // myPortfolio.push_back(new Swap(Date(2024, 1, 1), Date(2029, 1, 1), 2000000,
    //                                0.05, 1, true,
    //                                mkt, "USD-SOFR")); // Fixed-for-floating, annual
    // myPortfolio.push_back(new Swap(Date(2024, 1, 1), Date(2029, 1, 1), 2000000,
    //                                0.06, 2, false, 
    //                                mkt, "USD-SOFR")); // Floating-for-fixed, semi-annual

    // // Adding European Options
    // myPortfolio.push_back(
    //     new EuropeanOption(Call, 700, Date(2025, 12, 31), "AAPL")); // Call option
    // myPortfolio.push_back(
    //     new EuropeanOption(Put, 700, Date(2025, 12, 31), "AAPL")); // Put option

    // // Adding American Options
    // myPortfolio.push_back(
    //     new AmericanOption(Call, 700, Date(2025, 12, 31), "AAPL")); // Call option
    // myPortfolio.push_back(
    //     new AmericanOption(Put, 700, Date(2025, 12, 31), "AAPL")); // Put option

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
        logger.info("trade: " + trade->getType() + " " + trade->getUnderlying() + " PV : " + std::to_string(pv));
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
            logger.info("Comparing European Option: ");
            logger.info("Black-Scholes Price: " + std::to_string(bsPrice));
            logger.info("CRR Binomial Tree Price: " + std::to_string(crrPrice));
        }
    }

    // b) compare CRR binomial tree result for an American option vs European
    // option compare between US call / put vs EU call / put
    // for (auto trade : myPortfolio) {
    //     AmericanOption *amerOption = dynamic_cast<AmericanOption *>(trade);
    //     if (amerOption) {
    //         for (auto trade2 : myPortfolio) {
    //             EuropeanOption *euroOption =
    //                 dynamic_cast<EuropeanOption *>(trade2);
    //             if (euroOption &&
    //                 euroOption->getStrike() == amerOption->getStrike() &&
    //                 euroOption->GetExpiry() == amerOption->GetExpiry() &&
    //                 euroOption->getOptionType() ==
    //                     amerOption->getOptionType()) {
    //                 double amerPrice = treePricer->Price(mkt, amerOption);
    //                 double euroPrice = treePricer->Price(mkt, euroOption);
    //                 std::cout
    //                     << "Comparing American Option with European Option: "
    //                     << std::endl;
    //                 std::cout
    //                     << "*****American Option Price*****: " << amerPrice
    //                     << std::endl;
    //                 std::cout
    //                     << "*****European Option Price*****: " << euroPrice
    //                     << std::endl;
    //                 logger.info("Comparing American Option with European Option: ");
		  //           logger.info("*****American Option Price*****: " + std::to_string(amerPrice));
		  //           logger.info("*****European Option Price*****: " + std::to_string(euroPrice));
    //             }
    //         }
    //     }
    // }

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
	            for (auto* euroOption : europeanOptions) {
	                double euroPrice = treePricer->Price(mkt, euroOption);
	                std::cout << "Comparing American and European Options for Type: " << key.optionType
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

    // only relevant pairs of American and European options are compared,
    // rather than comparing every possible pair in the portfolio.
    // It leverages the structured nature of the SecurityKey to enforce that only matching options are compared.
 //    for (const auto& pair : securityMap) {
 //    	const SecurityKey& key = pair.first;
	//     const auto& americanOptions = pair.second.first;
	//     const auto& europeanOptions = pair.second.second;
 //        std::cout << "Key - Strike: " << key.strike << ", Expiry: " << key.expiry.toString() << std::endl;
 //        std::cout << "American options size: " << americanOptions.size() << std::endl;
 //        std::cout << "European options size: " << europeanOptions.size() << std::endl;

	//     for (auto amerOption : americanOptions) {
 //            Date amerExpiry = amerOption->GetExpiry();
 //            logger.info("Processing American Option. Underlying= " + amerOption->getUnderlying() +", Type= " + std::to_string(amerOption->getOptionType()) + ", Strike= " + std::to_string(amerOption->getStrike()) +
 //                    ", Expiry= " + amerExpiry.toString()); // !!!
        
	//         double amerPrice = treePricer->Price(mkt, amerOption);
	//         for (auto euroOption : europeanOptions) {
 //                std::cout<<"entered euro option loop"<<std::endl; //!!!
 //                Date euroExpiry = euroOption->GetExpiry();
 //                logger.info("Processing European Option. Underlying= " + euroOption->getUnderlying() +", Type= " + std::to_string(euroOption->getOptionType()) + ", Strike= " + std::to_string(euroOption->getStrike()) +
 //                    ", Expiry= " + euroExpiry.toString()); // !!!
 //                double euroPrice = treePricer->Price(mkt, euroOption);
	//             std::cout << "Comparing American Option with European Option: " << std::endl;
	//             std::cout << "*****American Option Price*****: " << amerPrice << std::endl;
	//             std::cout << "*****European Option Price*****: " << euroPrice << std::endl;
	//             logger.info("Comparing American Option with European Option: ");
	//             logger.info("*****American Option Price*****: " + std::to_string(amerPrice));
	//             logger.info("*****European Option Price*****: " + std::to_string(euroPrice));
	//         }
	//     }
	// }


    // final
    cout << "Project build successfully!" << endl;
    return 0;
}
