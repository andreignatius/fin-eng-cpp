#include "PortfolioMaker.h"

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

std::vector<std::unique_ptr<Trade>> PortfolioMaker::constructPortfolio(
    const Date &valueDate,
    const std::unordered_map<std::string, std::vector<std::string>>
        &portfolioMap,
    const Market &theMarket) {
    // construct portfolio here
    std::string type;
    std::string underlying;
    std::string start;
    std::string end;
    std::string opt;
    std::string freq_str;
    std::string notional_str;
    std::string strike_str;
    std::string id;
    double notional;
    double freq;
    double strike;
    Date startDate;
    Date endDate;
    Date expiryDate;
    bool fixed_for_float;
    std::vector<std::unique_ptr<Trade>> thePortfolio;
    std::vector<std::string> temp_parse;
    std::cout << "BEGIN PORTFOLIO CONSTRUCTION" << std::endl;
    printMap(portfolioMap);
    std::cout << portfolioMap.find("Id")->second.size() << std::endl;
    for (int i = 0; i < portfolioMap.find("Id")->second.size(); i++) {
        // 1. get the parameters
        id = portfolioMap.at("Id")[i];
        type = portfolioMap.find("type")->second[i];
        underlying = portfolioMap.find("underlying")->second[i];
        start = portfolioMap.find("start")->second[i];
        end = portfolioMap.find("end")->second[i];
        notional = std::stod(portfolioMap.find("notional")->second[i]);
        strike = std::stod(portfolioMap.find("strike")->second[i]);
        opt = portfolioMap.find("opt")->second[i];
        freq_str = portfolioMap.find("freq")->second[i];
        // intermediate output
        std::cout << i << " " << type << " " << underlying << " " << start
                  << " " << end << " " << notional << " " << strike << " "
                  << opt << " " << freq_str << " " << underlying << std::endl;
        // 1a. some dont need freq
        if (freq_str != "null") {
            freq = std::stod(freq_str);
        }
        // 1b. extract datetime if needed
        if (start != "null") {
            temp_parse = parseDelimiter(start, '/');
            startDate = Date(std::stoi(temp_parse[0]), std::stoi(temp_parse[1]),
                             std::stoi(temp_parse[2]));
        }
        if (end != "null") {
            temp_parse = parseDelimiter(end, '/');
            endDate = Date(std::stoi(temp_parse[0]), std::stoi(temp_parse[1]),
                           std::stoi(temp_parse[2]));
        }

        // 2. build the products
        if (type == "bond") {
            std::cout << "building BOND" << std::endl;
            thePortfolio.push_back(std::make_unique<Bond>(
                startDate, endDate, valueDate, notional, strike, freq,
                theMarket, underlying, "X"));
            std::cout << " ---> build complete" << std::endl;
        } else if (type == "swap") {
            std::cout << "building SWAP" << std::endl;
            if (notional < 0.0) {
                fixed_for_float = true;
            } else {
                fixed_for_float = false;
            }
            thePortfolio.push_back(std::make_unique<Swap>(
                startDate, endDate, valueDate, notional, strike, freq,
                fixed_for_float, theMarket, underlying, id));
            std::cout << " ---> build complete" << std::endl;

        } else if (type == "eur-opt") {
            std::cout << "building EURO OPT" << std::endl;
            if (opt == "call") {
                thePortfolio.push_back(std::make_unique<EuropeanOption>(
                    Call, strike, expiryDate, valueDate, underlying, id));
            } else if (opt == "put") {
                thePortfolio.push_back(std::make_unique<EuropeanOption>(
                    Put, strike, expiryDate, valueDate, underlying, id));
            } else {
                std::cout << "THIS IS NOT VALID OPTION TYPE" << std::endl;
            }
            std::cout << " ---> build complete" << std::endl;

        } else if (type == "am-opt") {
            std::cout << "building AMERICAN OPT" << std::endl;
            if (opt == "call") {
                thePortfolio.push_back(std::make_unique<AmericanOption>(
                    Call, strike, expiryDate, valueDate, underlying, id));
            } else if (opt == "put") {
                thePortfolio.push_back(std::make_unique<AmericanOption>(
                    Put, strike, expiryDate, valueDate, underlying, id));
            } else {
                std::cout << "THIS IS NOT VALID OPTION TYPE" << std::endl;
            }
            std::cout << " ---> build complete" << std::endl;

        } else {
            std::cout << "THIS IS NOT VALID PRODUCT" << std::endl;
        }
    }
    return thePortfolio;
}

std::vector<std::string> PortfolioMaker::parseDelimiter(const std::string &row,
                                                        const char &delim) {
    std::vector<std::string> result;
    std::stringstream ss(row);
    std::string item;
    while (getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}
