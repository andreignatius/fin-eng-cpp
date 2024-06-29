#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

#include "AmericanTrade.h"
#include "BlackScholesPricer.h"
#include "Bond.h"
#include "EuropeanTrade.h"
#include "JSONReader.h"
#include "Logger.h"
#include "Market.h"
#include "Pricer.h"
#include "Swap.h"
#include "Utils.h"

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

// Function to print the contents of the unordered_map
void printMap(
    const std::unordered_map<std::string, std::vector<std::string>> &map) {
    for (const auto &pair : map) {
        std::cout << pair.first << ": ";
        for (const auto &item : pair.second) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    // task 1, create an market data object, and update the market data from
    // from txt file
    std::filesystem::path DATA_PATH =
        std::filesystem::current_path() / "../../data";
    std::filesystem::path MKT_DATA_PATH =
        std::filesystem::current_path() / "../../market_data";
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
    vector<Trade *> thePortfolio;

    /*
        vector<Trade *> myPortfolio;

    JSONReader myJSONReader((MKT_DATA_PATH / "portfolio.json").string(), mkt,
                            myPortfolio);
    myJSONReader.constructPortfolio();
    myJSONReader.getMarketObject().Print();

    // why do i need to re-set myPortfolio?
    myPortfolio = myJSONReader.getPortfolio();
    std::unordered_map<SecurityKey, std::vector<Trade *>, SecurityHash>
        securityMap;

    for (auto &trade : myPortfolio) {
        // Check if the trade is an AmericanOption
        if (auto amerOption = dynamic_cast<AmericanOption *>(trade)) {
            SecurityKey key{amerOption->getOptionType(),
                            amerOption->getStrike(), amerOption->GetExpiry()};
            std::cout << "Inserting amer option: " << key.optionType << ", "
                      << key.strike << ", " << key.expiry << std::endl;
            securityMap[key].push_back(amerOption);
        }
        // Check if the trade is a EuropeanOption
        else if (auto euroOption = dynamic_cast<EuropeanOption *>(trade)) {
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
    */
    /*
    TESTING IF PARSER WORKS
    */
    std::unordered_map<std::string, std::vector<std::string>> testMap;
    CSVReader myCSVReader =
        CSVReader((MKT_DATA_PATH / "stock_price_20240601.csv").string());
    testMap = myCSVReader.parseFile();
    printMap(testMap);

    myCSVReader =
        CSVReader((MKT_DATA_PATH / "stock_price_20240602.csv").string());
    testMap = myCSVReader.parseFile();
    printMap(testMap);

    myCSVReader = CSVReader((MKT_DATA_PATH / "usd_sofr_20240601.csv").string());
    testMap = myCSVReader.parseFile();
    printMap(testMap);

    myCSVReader = CSVReader((MKT_DATA_PATH / "usd_sofr_20240602.csv").string());
    testMap = myCSVReader.parseFile();
    printMap(testMap);

    myCSVReader = CSVReader((MKT_DATA_PATH / "vol_20240601.csv").string());
    testMap = myCSVReader.parseFile();
    printMap(testMap);

    myCSVReader = CSVReader((MKT_DATA_PATH / "vol_20240602.csv").string());
    testMap = myCSVReader.parseFile();
    printMap(testMap);

    std::cout << "********************** TRY IT ON THE PORTFOLIO " << std::endl;
    myCSVReader = CSVReader((MKT_DATA_PATH / "portfolio.csv").string());
    testMap = myCSVReader.parseFile();
    printMap(testMap);

    // construct portfolio here
    std::string type;
    std::string underlying;
    std::string start;
    std::string end;
    std::string opt;
    double notional;
    double freq;
    double strike;
    Date startDate;
    Date endDate;
    Date expiryDate;
    std::vector<std::string> temp_parse;

    for (int i = 0; i < testMap["Id"].size(); i++) {
        std::cout << i << std::endl;
        type = testMap["type"][i];
        underlying = testMap["underlying"][i];

        if (type == "bond") {
            notional = std::stod(testMap["notional"][i]);
            strike = std::stod(testMap["strike"][i]);
            freq = std::stod(testMap["freq"][i]);
            std::cout << "building BOND" << std::endl;
            temp_parse = myCSVReader.parseRow(testMap["start"][i], '/');
            startDate = Date(std::stoi(temp_parse[0]), std::stoi(temp_parse[1]),
                             std::stoi(temp_parse[2]));
            temp_parse = myCSVReader.parseRow(testMap["end"][i], '/');
            endDate = Date(std::stoi(temp_parse[0]), std::stoi(temp_parse[1]),
                           std::stoi(temp_parse[2]));

        } else if (type == "swap") {
            notional = std::stod(testMap["notional"][i]);
            strike = std::stod(testMap["strike"][i]);
            freq = std::stod(testMap["freq"][i]);
            std::cout << "building SWAP" << std::endl;
            temp_parse = myCSVReader.parseRow(testMap["start"][i], '/');
            startDate = Date(std::stoi(temp_parse[0]), std::stoi(temp_parse[1]),
                             std::stoi(temp_parse[2]));
            temp_parse = myCSVReader.parseRow(testMap["end"][i], '/');
            endDate = Date(std::stoi(temp_parse[0]), std::stoi(temp_parse[1]),
                           std::stoi(temp_parse[2]));
            std::cout << startDate << " " << endDate << " " << notional << " "
                      << strike << " " << freq << std::endl;
            thePortfolio.push_back(new Swap(startDate, endDate, notional,
                                            strike, freq, true, mkt,
                                            testMap["underlying"][i], "x"));
        } else if (type == "eur-opt") {
            std::cout << "building EURO OPT" << std::endl;
            temp_parse = myCSVReader.parseRow(testMap["end"][i], '/');
            endDate = Date(std::stoi(temp_parse[0]), std::stoi(temp_parse[1]),
                           std::stoi(temp_parse[2]));
            std::cout << startDate << " " << endDate << " " << testMap["opt"][i]
                      << " " << strike << " " << freq << std::endl;
            if (testMap["opt"][i] == "call") {
                thePortfolio.push_back(
                    new EuropeanOption(Call, stod(testMap["strike"][i]),
                                       endDate, underlying, "x"));
            } else if (testMap["opt"][i] == "put") {
                thePortfolio.push_back(new EuropeanOption(
                    Put, stod(testMap["strike"][i]), endDate, underlying, "x"));
            } else {
                std::cout << "THIS IS NOT VALID OPTION TYPE" << std::endl;
            }
        } else if (type == "am-opt") {
            std::cout << "building AMERICAN OPT" << std::endl;
            temp_parse = myCSVReader.parseRow(testMap["end"][i], '/');
            endDate = Date(std::stoi(temp_parse[0]), std::stoi(temp_parse[1]),
                           std::stoi(temp_parse[2]));
            std::cout << startDate << " " << endDate << " " << testMap["opt"][i]
                      << " " << strike << " " << freq << std::endl;
            if (testMap["opt"][i] == "call") {
                thePortfolio.push_back(
                    new AmericanOption(Call, std::stof(testMap["strike"][i]),
                                       endDate, underlying, "x"));
            } else if (testMap["opt"][i] == "put") {
                thePortfolio.push_back(
                    new AmericanOption(Put, std::stof(testMap["strike"][i]),
                                       endDate, underlying, "x"));
            } else {
                std::cout << "THIS IS NOT VALID OPTION TYPE" << std::endl;
            }

        } else {
            std::cout << "THIS IS NOT VALID PRODUCT" << std::endl;
        }
    }

    return 0;
}
