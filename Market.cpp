#include "Market.h"
#include "Constants.h"
#include <unordered_map>

void RateCurve::display() const {
    std::cout << "Rate curve for: " << name
              << "; Start Date :" << this->startDate << std::endl;
    for (size_t i = 0; i < tenors.size(); i++) {
        std::cout << "Tenor: " << tenors[i] << " Rate: " << rates[i]
                  << std::endl;
    }
    std::cout << std::endl;
}
void RateCurve::addRate(const Date &tenor, double rate) {
    // consider to check if tenor already exist
    // Search for the tenor in the existing list of tenors
    auto it = std::find_if(
        tenors.begin(), tenors.end(), [&tenor](const Date &existingTenor) {
            return existingTenor == tenor; // Assuming you have an equality
                                           // operator defined for Date
        });

    if (it != tenors.end()) {
        // Tenor already exists, replace the rate
        auto index = std::distance(tenors.begin(), it);
        rates[index] = rate;
        std::cout << "Updated existing tenor " << tenor << " with new rate "
                  << rate << "." << std::endl;
    } else {
        // Tenor does not exist, add new tenor and rate
        tenors.push_back(tenor);
        rates.push_back(rate);
        std::cout << "Added new tenor " << tenor << " with rate " << rate << "."
                  << std::endl;
    }
}

// JOS: assumes continuous compound zero rates are given.
double RateCurve::getRate(Date tenor) const {
    if (tenors.empty())
        return 0; // No rates added

    if (tenor <= tenors[0]) {
        return rates[0]; // if tenor date is shorter than available tenors
    } else if (tenor >= tenors[tenors.size() - 1]) {
        return rates[rates.size() -
                     1]; // if tenor date is longer than available tenors
    } else {
        // Linear interpolation
        // explore better way to do this than for loop maybe
        for (size_t i = 1; i < tenors.size(); ++i) {
            if (tenors[i] >= tenor) {
                // Perform interpolation
                double lowerDay =
                    (tenors[i - 1].differenceInDays(this->startDate)) /
                    Constants::NUM_DAYS_IN_YEAR;
                double upperDay =
                    (tenors[i].differenceInDays(this->startDate)) /
                    Constants::NUM_DAYS_IN_YEAR;
                double Day = tenor.differenceInDays(this->startDate) /
                             Constants::NUM_DAYS_IN_YEAR;

                double lowerDf = exp(-rates[i - 1] * lowerDay);
                double upperDf = exp(-rates[i] * upperDay);

                double interpDf = lowerDf + (upperDf - lowerDf) *
                                                (Day - lowerDay) /
                                                (upperDay - lowerDay);
                return -log(interpDf) / (Day);
            }
        }
    }
    return 0;
}

std::vector<double> &RateCurve::getRates() { return this->rates; }
std::vector<Date> &RateCurve::getTenors() { return this->tenors; }

std::vector<double> &VolCurve::getRates() { return this->vols; }
std::vector<Date> &VolCurve::getTenors() { return this->tenors; }

void VolCurve::addVol(Date tenor, double volInDecimal) {
    // consider to check if tenor already exist
    // Search for the tenor in the existing list of tenors
    auto it = std::find_if(
        tenors.begin(), tenors.end(), [&tenor](const Date &existingTenor) {
            return existingTenor == tenor; // Assuming you have an equality
                                           // operator defined for Date
        });

    if (it != tenors.end()) {
        // Tenor already exists, replace the rate
        auto index = std::distance(tenors.begin(), it);
        vols[index] = volInDecimal;
        std::cout << "Updated existing tenor " << tenor << " with new vol "
                  << volInDecimal << std::endl;
    } else {
        // Tenor does not exist, add new tenor and rate
        tenors.push_back(tenor);
        vols.push_back(volInDecimal);
        std::cout << "Added new tenor " << tenor << " with vol " << volInDecimal
                  << std::endl;
    }
}

// TODO : What is this volatility even? Volatility curve like the smile?
double VolCurve::getVol(Date tenor) const {
    if (tenors.empty())
        return 0; // No vols added

    if (tenor <= tenors[0]) {
        return vols[0]; // if tenor date is shorter than available tenors
    } else if (tenor >= tenors[tenors.size() - 1]) {
        return vols[vols.size() -
                    1]; // if tenor date is longer than available tenors
    } else {
        // Linear interpolation
        // explore better way to do this than for loop maybe
        for (size_t i = 1; i < tenors.size(); ++i) {
            if (tenors[i] >= tenor) {
                // Perform interpolation
                double diffVol = vols[i] - vols[i - 1];
                double tenorStep = tenors[i].differenceInDays(tenors[i - 1]);
                double tenorDiff = tenor.differenceInDays(tenors[i - 1]);
                double interpVol =
                    vols[i - 1] + diffVol * (tenorDiff / tenorStep);
                return interpVol;
            }
        }
    }
    return 0;
}

vector<double> VolCurve::getVols() const { return this->vols; }

double VolCurve::getLatestVol() const {
    if (!vols.empty()) {
        return vols.back(); // Return the last element in the volatility vector
    } else {
        return 0; // Return a default value if there are no entries
    }
}

void VolCurve::display() const {
    std::cout << "Volatility curve for: " << name
              << "; Start Date :" << this->startDate << std::endl;
    for (size_t i = 0; i < tenors.size(); i++) {
        std::cout << "Tenor: " << tenors[i] << " Vol: " << vols[i] << std::endl;
    }
    std::cout << std::endl;
}

void Market::Print() const {
    std::cout << "============= PRINT MARKET START =============" << std::endl;
    std::cout << "market asof: " << asOf << std::endl;

    // LR: should we have similar methods for displaying them?
    for (auto &curve : rateCurves) {
        curve.second.display();
    }
    for (auto &vol : volCurves) {
        vol.second.display();
    }

    // Add display for bond price and stock price
    // TODO LR: bond output is not shown when running
    std::cout << "Bond Prices:" << std::endl;
    for (const auto &bond : bondPrices) {
        std::cout << "Bond: " << bond.first << " Price: " << bond.second
                  << std::endl;
    }
    std::cout << "Stock Prices:" << std::endl;
    // for (const auto &stock : stockPrices) {
    //     std::cout << "Stock: " << stock.first << " Price: " << stock.second
    //               << std::endl;
    // }
    // Outer loop iterates over each date
    for (const auto &dateEntry : dailyStockPrices) {
        const Date &date = dateEntry.first;    // This gets the date
        const auto &stocks = dateEntry.second; // This gets the map of stocks
                                               // and their prices for that date

        std::cout << "Date: " << date.toString() << std::endl;

        // Inner loop iterates over each stock and its price for the given date
        for (const auto &stock : stocks) {
            std::cout << " Stock: " << stock.first << " Price: " << stock.second
                      << std::endl;
        }
    }

    std::cout << "Asset Mapping:" << std::endl;
    for (const auto &item : assetTypes) {
        std::cout << "Asset Name: " << item.first
                  << ", Asset Type: " << this->assetTypeToString(item.second)
                  << std::endl;
    }

    std::cout << "============= PRINT MARKET END =============" << std::endl;
    std::cout << std::endl;
}

void Market::addCurve(const std::string &curveName, const RateCurve &curve) {
    rateCurves.emplace(curveName, curve);
}

void Market::addVolCurve(const std::string &curveName, const VolCurve &curve) {
    volCurves[curveName] = curve;
}

void Market::setRiskFreeRate(double rate) { riskFreeRate = rate; }

void Market::addAssetType(const std::string &assetName, AssetType type) {
    assetTypes[assetName] = type;
}

void Market::addBondPrice(const std::string &bondName, double price) {
    bondPrices[bondName] = price;
}

// void Market::addStockPrice(const std::string &stockName, double price) {
//     stockPrices[stockName] = price;
// }

// RateCurve Market::getCurve(const string& name) const {
//     auto defaultIt = this->rateCurves.find(name);
//     if (defaultIt != this->rateCurves.end()) {
//         return defaultIt->second;
//     } else{
//         throw std::invalid_argument("invalid rate curve name supplied");
//     }
// }

// // Retrieve curve or price information
// RateCurve Market::getCurve(const Date& date, const string& curveName) const {
//     std::cout << "try to getCurve: " << date.toString() << ", " << curveName
//     << std::endl; auto itDate = dailyCurves.find(date); if (itDate !=
//     dailyCurves.end()) {
//         std::cout << "able to find " << date.toString() << " in dailyCurves"
//         << std::endl; std::cout << "checking111" << itDate->first.toString()
//         << std::endl;
//         // "," << itDate->second.first.toString() << std::endl;
//         for (const auto& curvePair : itDate->second) {
//             std::cout << "  Asset: " << curvePair.first << std::endl;
//             curvePair.second.display();
//             // curvePair.second.display();  // Assuming RateCurve has a
//             display() method
//         }
//         auto itCurve = itDate->second.find(curveName);
//         if (itCurve != itDate->second.end()) {
//             return itCurve->second;
//         }
//     }
//     throw runtime_error("Rate Curve not found for given date and name.");
// }

// Retrieve curve or price information
RateCurve Market::getCurve(const Date &date, const string &curveName) const {
    // std::cout << "try to getCurve: " << date.toString() << ", " << curveName
    //           << std::endl;
    auto itDate = dailyCurves.find(date);
    if (itDate != dailyCurves.end()) {

        // Display all curves available on this date
        /*
        for (const auto &curvePair : itDate->second) {
             std::cout << "  Available Asset: " << curvePair.first <<
             std::endl;
              curvePair.second
                  .display(); // Assuming RateCurve has a display() method
        }
        */

        auto itCurve = itDate->second.find(curveName);
        if (itCurve != itDate->second.end()) {
            return itCurve->second;
        } else {
            std::cerr << "Curve not found: " << curveName << " on "
                      << date.toString() << std::endl;
        }
    } else {
        std::cerr << "Date not found in dailyCurves: " << date.toString()
                  << std::endl;
    }
    throw runtime_error("Rate Curve not found for given date and name: " +
                        curveName + " on " + date.toString());
}

// VolCurve Market::getVolCurve(const string& name) const {

//     auto defaultIt = this->volCurves.find(name);
//     if (defaultIt != this->volCurves.end()) {
//         return defaultIt->second;
//     } else{
//         throw std::invalid_argument("invalid vol curve supplied");
//     }
// }

VolCurve Market::getVolCurve(const Date &date, const string &curveName) const {
    auto itDate = dailyVolCurves.find(date);
    if (itDate != dailyVolCurves.end()) {
        auto itCurve = itDate->second.find(curveName);
        if (itCurve != itDate->second.end()) {
            return itCurve->second;
        }
    }
    throw runtime_error("Vol Curve not found for given date and name.");
}

//*******************

// void Market::updateMarketFromVolFile(const std::string &filePath,
//                                      const std::string &volName) {
//     VolCurve volCurve(volName, this->asOf); // You can dynamically name it
//     based
//                                             // on some criteria if needed

//     if (filePath.find(".csv") != std::string::npos) {
//         std::unordered_map<std::string, std::vector<std::string>> volMap;
//         CSVReader myCSVReader = CSVReader(filePath);
//         volMap = myCSVReader.parseFile();

//         // further processing after parsing the csv file
//         for (int i = 0; i < volMap["tenor"].size(); i++) {
//             double rateToDecimal = std::stod(volMap["vol"][i].substr(
//                                        0, volMap["vol"][i].size() - 1)) /
//                                    100;
//             double mappedTenorMonths = tenorMap[volMap["tenor"][i]];
//             Date tenorDate = this->asOf; // Use the market's current date
//             tenorDate.addMonths(
//                 mappedTenorMonths); // our Date class uses TM which takes
//                                     // integer for year and month, so we work
//                                     // with months
//             volCurve.addVol(tenorDate, rateToDecimal);
//         }
//     } else {

//         std::ifstream file(filePath);
//         if (!file.is_open()) {
//             std::cerr << "Failed to open file: " << filePath << std::endl;
//             return;
//         }

//         std::string line;

//         while (getline(file, line)) {
//             std::istringstream iss(line);
//             std::string tenor;
//             double vol;
//             char colon;
//             char percent;

//             if (!(iss >> tenor >> colon >> vol >> percent)) {
//                 std::cerr << "Failed to parse line: " << line << std::endl;
//                 continue; // Skip malformed lines
//             }

//             tenor.pop_back(); // Remove last character ('M' or 'Y')
//             int numMonths = std::stoi(tenor);
//             if (line.back() == 'Y') {
//                 numMonths *= 12; // Convert years to months if necessary
//             }

//             Date tenorDate = asOf;
//             tenorDate.addMonths(numMonths); // Method to add months to Date

//             volCurve.addVol(tenorDate,
//                             vol / 100.0); // Convert percentage to decimal
//                             and
//                                           // add to vol curve
//         }
//         file.close();
//     }

//     this->addVolCurve(volName, volCurve); // Adding the vol curve to the
//     market
// }

void Market::updateMarketFromVolFile(const std::string &filePath,
                                     const std::string &volName,
                                     const Date &specificDate) {
    VolCurve volCurve(volName,
                      specificDate); // Use specificDate instead of this->asOf

    if (filePath.find(".csv") != std::string::npos) {
        std::unordered_map<std::string, std::vector<std::string>> volMap;
        CSVReader myCSVReader = CSVReader(filePath);
        volMap = myCSVReader.parseFile();

        for (int i = 0; i < volMap["tenor"].size(); i++) {
            double volToDecimal = std::stod(volMap["vol"][i].substr(
                                      0, volMap["vol"][i].size() - 1)) /
                                  100;
            double mappedTenorMonths = tenorMap[volMap["tenor"][i]];
            Date tenorDate = specificDate;
            tenorDate.addMonths(mappedTenorMonths);
            volCurve.addVol(tenorDate, volToDecimal);
        }
    } else {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            return;
        }
        std::string line;

        while (getline(file, line)) {
            std::istringstream iss(line);
            std::string tenor;
            double vol;
            char colon;
            char percent;

            if (!(iss >> tenor >> colon >> vol >> percent)) {
                std::cerr << "Failed to parse line: " << line << std::endl;
                continue; // Skip malformed lines
            }

            tenor.pop_back(); // Remove last character ('M' or 'Y')
            int numMonths = std::stoi(tenor);
            if (line.back() == 'Y') {
                numMonths *= 12; // Convert years to months if necessary
            }

            Date tenorDate = specificDate;
            tenorDate.addMonths(numMonths); // Method to add months to Date

            volCurve.addVol(tenorDate,
                            vol / 100.0); // Convert percentage to decimal and
                                          // add to vol curve
        }
        file.close();
    }

    // Ensure the map for the specific date exists and then add or update the
    // vol curve
    dailyVolCurves[specificDate][volName] = volCurve;
}

void Market::updateMarketFromBondFile(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open bond price file: " << filePath
                  << std::endl;
        return;
    }

    std::string line;
    while (getline(file, line)) {
        // Trimming spaces around the line if needed
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace),
                   line.end());

        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            std::cerr << "Malformed line (no colon found): " << line
                      << std::endl;
            continue; // Skip malformed lines
        }

        std::string bondName = line.substr(0, colonPos);
        std::string priceStr = line.substr(colonPos + 1);

        double price;
        try {
            price = std::stod(priceStr);
        } catch (const std::exception &e) {
            std::cerr << "Failed to convert price to double: " << priceStr
                      << " in line: " << line << std::endl;
            continue; // Skip lines with conversion errors
        }
        std::cout << "adding bond: " << bondName
                  << " where price is : " << price << std::endl;
        addBondPrice(bondName, price);
    }
    file.close();
}

// JOS : assumes input csv contains 2 columns : stock | price
// void Market::updateMarketFromStockFile(const std::string &filePath) {

//     // if user supplies CSV File
//     if (filePath.find(".csv") != std::string::npos) {
//         std::unordered_map<std::string, std::vector<std::string>> stockMap;
//         CSVReader myCSVReader = CSVReader(filePath);
//         stockMap = myCSVReader.parseFile();
//         for (int i = 0; i < stockMap["stock"].size(); i++) {
//             std::cout << "adding stock : " << stockMap["stock"][i] <<
//             std::endl; addStockPrice(stockMap["stock"][i],
//                           std::stod(stockMap["price"][i]));
//         }
//         // if user supplies TXT File
//     } else {
//         std::ifstream file(filePath);
//         if (!file.is_open()) {
//             std::cerr << "Failed to open stock price file: " << filePath
//                       << std::endl;
//             return;
//         }

//         std::string line;
//         while (getline(file, line)) {
//             // Remove spaces around the line if needed
//             line.erase(std::remove_if(line.begin(), line.end(), ::isspace),
//                        line.end());

//             size_t colonPos = line.find(':');
//             if (colonPos == std::string::npos) {
//                 std::cerr << "Malformed line (no colon found): " << line
//                           << std::endl;
//                 continue; // Skip malformed lines
//             }

//             std::string assetName = line.substr(0, colonPos);
//             std::string priceStr = line.substr(colonPos + 1);

//             double price;
//             try {
//                 price = std::stod(priceStr);
//             } catch (const std::exception &e) {
//                 std::cerr << "Failed to convert price to double: " <<
//                 priceStr
//                           << " in line: " << line << std::endl;
//                 continue; // Skip lines with conversion errors
//             }

//             addStockPrice(assetName, price);
//         }
//         file.close();
//     }
// }

void Market::updateMarketFromStockFile(const std::string &filePath,
                                       const Date &specificDate) {

    // if user supplies CSV File
    if (filePath.find(".csv") != std::string::npos) {
        std::unordered_map<std::string, std::vector<std::string>> stockMap;
        CSVReader myCSVReader = CSVReader(filePath);
        stockMap = myCSVReader.parseFile();
        for (int i = 0; i < stockMap["stock"].size(); i++) {
            std::cout << "adding stock : " << stockMap["stock"][i] << std::endl;
            // Add stock price for the specific date
            addAssetType(
                stockMap["stock"][i],
                AssetType::Stock); // Add type information when adding stock
            dailyStockPrices[specificDate][stockMap["stock"][i]] =
                std::stod(stockMap["price"][i]);
        }
    } else { // if user supplies TXT File
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open stock price file: " << filePath
                      << std::endl;
            return;
        }

        std::string line;
        while (getline(file, line)) {
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace),
                       line.end());

            size_t colonPos = line.find(':');
            if (colonPos == std::string::npos) {
                std::cerr << "Malformed line (no colon found): " << line
                          << std::endl;
                continue; // Skip malformed lines
            }

            std::string assetName = line.substr(0, colonPos);
            std::string priceStr = line.substr(colonPos + 1);

            double price;
            try {
                price = std::stod(priceStr);
            } catch (const std::exception &e) {
                std::cerr << "Failed to convert price to double: " << priceStr
                          << " in line: " << line << std::endl;
                continue; // Skip lines with conversion errors
            }

            // Add stock price for the specific date
            addAssetType(
                assetName,
                AssetType::Stock); // Add type information when adding stock
            dailyStockPrices[specificDate][assetName] = price;
        }
        file.close();
    }
}

// // JOS : assumes input csv contains 2 columns : tenor | rate
// void Market::updateMarketFromCurveFile(const std::string &filePath,
//                                        const std::string &curveName) {

//     RateCurve rateCurve(curveName, this->asOf);

//     if (filePath.find(".csv") != std::string::npos) {
//         std::unordered_map<std::string, std::vector<std::string>> curveMap;
//         CSVReader myCSVReader = CSVReader(filePath);
//         curveMap = myCSVReader.parseFile();

//         // further processing after parsing the csv file
//         for (int i = 0; i < curveMap["tenor"].size(); i++) {
//             double rateToDecimal = std::stod(curveMap["rate"][i].substr(
//                                        0, curveMap["rate"][i].size() - 1)) /
//                                    100;
//             double mappedTenorMonths = tenorMap[curveMap["tenor"][i]];
//             Date tenorDate = this->asOf; // Use the market's current date
//             tenorDate.addMonths(
//                 mappedTenorMonths); // our Date class uses TM which takes
//                                     // integer for year and month, so we work
//                                     // with months
//             rateCurve.addRate(tenorDate, rateToDecimal);
//         }

//     } else {
//         std::ifstream file(filePath);
//         if (!file.is_open()) {
//             std::cerr << "Failed to open file: " << filePath << std::endl;
//             return;
//         }
//         std::string line;
//         getline(file, line); // skip header line of curve.txt

//         while (getline(file, line)) {
//             std::istringstream iss(line);
//             double rate;
//             char delimiter;

//             // Find the position of the colon
//             size_t colonPos = line.find(':');
//             if (colonPos == std::string::npos) {
//                 std::cout << "Failed to find colon in line: " << line <<
//                 std::endl; continue; // Skip this line if no colon found
//             }

//             // Split the line into tenorStr and rateStr assuming a space
//             after the colon std::string tenor = line.substr(0, colonPos);
//             std::string rateStr = line.substr(colonPos + 2); // +2 to skip ":
//             " rateStr.pop_back(); rate=std::stod(rateStr)/100.0; // Convert
//             percentage to decimal
//             // Remove last character ('M' or 'Y') and calculate months
//             int numMonths;
//             if (tenor == "ON") {
//                 numMonths = 0;  // Overnight doesn't add months
//             } else {
//                 numMonths = std::stoi(tenor.substr(0, tenor.size() - 1));
//                 if (tenor.back() == 'Y') {
//                     numMonths *= 12;
//                 }
//             }
//             Date tenorDate = this->asOf; // Use the market's current date
//             tenorDate.addMonths(numMonths);
//             rateCurve.addRate(tenorDate, rate);
//         }
//         file.close();
//     }
//     this->addCurve(
//         curveName,
//         rateCurve); // Use the existing method to add the curve to the market
// }

void Market::updateMarketFromCurveFile(
    const std::string &filePath, const std::string &curveName,
    const Date &specificDate) { // Add Date parameter to specify which day's
                                // data is being updated

    RateCurve rateCurve(curveName,
                        specificDate); // Use specificDate instead of this->asOf

    if (filePath.find(".csv") != std::string::npos) {
        std::unordered_map<std::string, std::vector<std::string>> curveMap;
        CSVReader myCSVReader = CSVReader(filePath);
        curveMap = myCSVReader.parseFile();

        for (int i = 0; i < curveMap["tenor"].size(); i++) {
            double rateToDecimal = std::stod(curveMap["rate"][i].substr(
                                       0, curveMap["rate"][i].size() - 1)) /
                                   100;
            double mappedTenorMonths = tenorMap[curveMap["tenor"][i]];
            Date tenorDate = specificDate;
            tenorDate.addMonths(mappedTenorMonths);
            rateCurve.addRate(tenorDate, rateToDecimal);
        }
    } else {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            return;
        }
        std::string line;
        getline(file, line); // skip header line

        while (getline(file, line)) {
            std::istringstream iss(line);
            std::string tenor;
            double rate;
            char delimiter;

            size_t colonPos = line.find(':');
            if (colonPos == std::string::npos) {
                std::cerr << "Failed to find colon in line: " << line
                          << std::endl;
                continue;
            }

            std::string tenorStr = line.substr(0, colonPos);
            std::string rateStr = line.substr(colonPos + 2); // Skip ": "
            rateStr.pop_back(); // Remove the '%' or any trailing character
            rate = std::stod(rateStr) / 100.0; // Convert percentage to decimal

            int numMonths =
                tenorStr == "ON"
                    ? 0
                    : std::stoi(tenorStr.substr(0, tenorStr.size() - 1));
            if (tenorStr.back() == 'Y') {
                numMonths *= 12; // Convert years to months
            }

            Date tenorDate = specificDate;
            tenorDate.addMonths(numMonths);
            rateCurve.addRate(tenorDate, rate);
        }
        file.close();
    }
    addAssetType(curveName,
                 AssetType::Rate); // Add type information when adding curves
    // Ensure the map for the specific date exists and then add or update the
    // rate curve
    dailyCurves[specificDate][curveName] = rateCurve;
}

// TODO : get spot price, get vol and get rate implementation needs to be
// refactored JOS: Get spot looks ok
// double Market::getSpotPrice(const std::string &assetName) const {
//     auto it = stockPrices.find(assetName);
//     if (it != stockPrices.end()) {
//         return it->second;
//     }
//     std::cerr << "Asset not found: " << assetName
//               << ", returning default price 0." << std::endl;
//     return 0; // Return a default price or handle as needed
// }
double Market::getSpotPrice(const std::string &assetName,
                            const Date &specificDate) const {
    // First, find the date entry in the dailyStockPrices
    auto dateIt = dailyStockPrices.find(specificDate);
    if (dateIt != dailyStockPrices.end()) {
        // Now find the asset in the map of stock prices for this date
        auto stockIt = dateIt->second.find(assetName);
        if (stockIt != dateIt->second.end()) {
            return stockIt->second; // Return the found price
        } else {
            std::cerr << "Stock Asset not found: " << assetName << " on "
                      << specificDate.toString()
                      << ", returning default price 0." << std::endl;
        }
    } else {
        std::cerr << "Date not found: " << specificDate.toString()
                  << ", returning default price 0." << std::endl;
    }
    return 0; // Return a default price or handle as needed
}

std::string Market::dateToTenor(const Date &startDate,
                                const Date &endDate) const {
    int monthsDiff =
        startDate.monthsUntil(endDate); // You need to implement monthsUntil
    if (monthsDiff % 12 == 0) {
        // If the total months difference is exactly in years
        return std::to_string(monthsDiff / 12) + "Y";
    } else {
        return std::to_string(monthsDiff) + "M";
    }
}

// double Market::getCurveRate(const std::string &assetName, const Date
// &specificDate) const {
//     // First, find the date entry in the dailyStockPrices
//     auto dateIt = dailyCurves.find(specificDate);
//     if (dateIt != dailyCurves.end()) {
//         // Now find the asset in the map of stock prices for this date
//         auto stockIt = dateIt->second.find(assetName);
//         if (stockIt != dateIt->second.end()) {
//             return stockIt->second;  // Return the found price
//         } else {
//             std::cerr << "Asset not found: " << assetName << " on " <<
//             specificDate.toString()
//                       << ", returning default price 0." << std::endl;
//         }
//     } else {
//         std::cerr << "Date not found: " << specificDate.toString()
//                   << ", returning default price 0." << std::endl;
//     }
//     return 0; // Return a default price or handle as needed
// }

double Market::getCurveRate(const std::string &assetName,
                            const Date &specificDate) const {
    auto dateIt = dailyCurves.find(Date(2024, 6, 1));
    if (dateIt != dailyCurves.end()) {
        auto curveIt = dateIt->second.find(assetName);
        if (curveIt != dateIt->second.end()) {
            // // Assume you have a start date for the curve, which needs to be
            // known or stored Date curveStartDate = curveIt->second.startDate;
            // std::string tenor = dateToTenor(curveStartDate, specificDate);

            try {
                return curveIt->second.getRate(specificDate);
            } catch (const std::exception &e) {
                std::cerr << "Error getting rate for date: "
                          << specificDate.toString() << " - " << e.what()
                          << std::endl;
            }
        } else {
            std::cerr << "Curve not found: " << assetName << " on "
                      << specificDate.toString()
                      << ", returning default price 0." << std::endl;
        }
    } else {
        std::cerr << "Date not found: " << specificDate.toString()
                  << ", returning default price 0." << std::endl;
    }
    return 0; // Default price if no data found
}

double Market::getPriceOrRate(const std::string &assetName,
                              const Date &date) const {
    std::cout << "getPriceOrRate for : " << assetName << std::endl;
    AssetType type = getAssetType(assetName); // Find out the type of the asset
    std::cout << "checking if stock or rate: " << assetName << " --> "
              << assetTypeToString(type) << std::endl;
    switch (type) {
    case AssetType::Rate:
        std::cout << "getPriceOrRate : GETTING RATE" << std::endl;
        return getCurveRate(
            assetName, date); // Implement this function to fetch curve data
    case AssetType::Stock:
        std::cout << "getPriceOrRate : GETTING STOCK" << std::endl;
        return getSpotPrice(assetName, date); // Use your existing function
    default:
        throw std::runtime_error("Unknown asset type for: " + assetName);
    }
}

double Market::getBondPrice(const std::string &assetName) const {
    auto it = bondPrices.find(assetName);
    if (it != bondPrices.end()) {
        return it->second;
    }
    std::cerr << "Bond asset not found: " << assetName
              << ", returning default price 0." << std::endl;
    return 0; // Return a default price or handle as needed
}

double Market::getVolatility(const std::string &assetName) const {
    auto it = volCurves.find(assetName);
    if (it != volCurves.end()) {
        return it->second.getLatestVol();
    }
    std::cerr << "Volatility data not found for asset: " << assetName
              << ", returning default vol 0." << std::endl;
    return 0; // Return a default value or handle as needed
}

double Market::getRiskFreeRate() const {
    // Assuming risk-free rate is a single value for simplicity; implement
    // accordingly
    return riskFreeRate;
}

AssetType Market::getAssetType(const std::string &assetName) const {
    auto it = assetTypes.find(assetName);
    if (it != assetTypes.end()) {
        return it->second;
    }
    throw std::runtime_error("Asset type not found for: " + assetName);
}

std::string Market::assetTypeToString(AssetType type) const {
    switch (type) {
    case AssetType::Rate:
        return "Rate";
    case AssetType::Stock:
        return "Stock";
    default:
        return "Unknown";
    }
}

void Market::adjustInterestRates(double delta) {
    for (auto &[key, curve] : rateCurves) {
        for (auto &rate : curve.getRates()) {
            std::cout << "111rate before: " << rate << std::endl;
            rate += delta;
        }
    }

    for (auto &[key, curve] : rateCurves) {
        for (auto &rate : curve.getRates()) {
            std::cout << "111rate after: " << rate << std::endl;
        }
    }
}

void Market::adjustVolatility(const std::string &underlying, double delta) {
    for (auto &[key, volCurve] : volCurves) {
        if (key == underlying) {
            for (auto &vol : volCurve.getVols()) {
                vol += delta;
            }
        }
    }
}

std::ostream &operator<<(std::ostream &os, const Market &mkt) {
    os << mkt.asOf << std::endl;
    return os;
}

std::istream &operator>>(std::istream &is, Market &mkt) {
    is >> mkt.asOf;
    return is;
}
