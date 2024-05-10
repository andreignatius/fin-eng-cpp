#include "Market.h"
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

void RateCurve::display() const {
    cout << "rate curve:" << name << endl;
    for (size_t i = 0; i < tenors.size(); i++) {
        cout << tenors[i] << ":" << rates[i] << endl;
    }
    cout << endl;
}

void RateCurve::addRate(const Date& tenor, double rate) {
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
        cout << "Updated existing tenor " << tenor << " with new rate " << rate
             << "." << endl;
    } else {
        // Tenor does not exist, add new tenor and rate
        tenors.push_back(tenor);
        rates.push_back(rate);
        cout << "Added new tenor " << tenor << " with rate " << rate << "."
             << endl;
    }
}

double RateCurve::getRate(Date tenor) const {
    if (tenors.empty())
        return 0; // No rates added

    // Linear interpolation
    for (size_t i = 1; i < tenors.size(); ++i) {
        if (tenors[i] >= tenor) {
            // Perform interpolation
            double rateDiff = rates[i] - rates[i - 1];
            double tenorDiff = tenors[i].differenceInDays(tenors[i - 1]);
            double tenorStep = tenor.differenceInDays(tenors[i - 1]);
            return rates[i - 1] + (rateDiff / tenorDiff) * tenorStep;
        }
    }
    // If the tenor is beyond known rates, extrapolate the last rate
    return rates.back();
}

void VolCurve::addVol(Date tenor, double vol) {
    tenors.push_back(tenor);
    vols.push_back(vol);
}

double VolCurve::getVol(Date tenor) const {
    if (tenors.empty())
        return 0; // No vols added

    // Linear interpolation
    for (size_t i = 1; i < tenors.size(); ++i) {
        if (tenors[i] >= tenor) {
            // Perform interpolation
            double volDiff = vols[i] - vols[i - 1];
            double tenorDiff = tenors[i].differenceInDays(tenors[i - 1]);
            double tenorStep = tenor.differenceInDays(tenors[i - 1]);
            return vols[i - 1] + (volDiff / tenorDiff) * tenorStep;
        }
    }
    // If the tenor is beyond known vols, extrapolate the last vol
    return vols.back();
}

double VolCurve::getLatestVol() const {
    if (!vols.empty()) {
        return vols.back(); // Return the last element in the volatility vector
    } else {
        return 0; // Return a default value if there are no entries
    }
}

void VolCurve::display() const {
    cout << "Volatility curve for: " << name << endl;
    for (size_t i = 0; i < tenors.size(); i++) {
        cout << "Tenor: " << tenors[i] << " Vol: " << vols[i] << "%" << endl;
    }
}

void Market::Print() const {
    cout << "market asof: " << asOf << endl;

    for (auto &curve : curves) {
        curve.second.display();
    }
    for (auto &vol : vols) {
        vol.second.display();
    }

    // Add display for bond price and stock price
    cout << "Bond Prices:" << endl;
    for (const auto &bond : bondPrices) {
        cout << "Bond: " << bond.first << " Price: " << bond.second << endl;
    }
    cout << "Stock Prices:" << endl;
    for (const auto &stock : stockPrices) {
        cout << "Stock: " << stock.first << " Price: " << stock.second << endl;
    }
}

void Market::addCurve(const std::string &curveName, const RateCurve &curve) {
    curves.emplace(curveName, curve);
}

void Market::addVolCurve(const std::string &curveName, const VolCurve &curve) {
    vols[curveName] = curve;
}

void Market::setRiskFreeRate(double rate) { riskFreeRate = rate; }

void Market::addBondPrice(const std::string &bondName, double price) {
    bondPrices[bondName] = price;
}

void Market::addStockPrice(const std::string &stockName, double price) {
    stockPrices[stockName] = price;
}

void Market::updateMarketFromVolFile(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    std::string line;
    VolCurve volCurve("VolCurve1"); // You can dynamically name it based on some
                                    // criteria if needed

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

        Date tenorDate = asOf;
        tenorDate.addMonths(numMonths); // Method to add months to Date

        volCurve.addVol(
            tenorDate,
            vol / 100.0); // Convert percentage to decimal and add to vol curve
    }

    addVolCurve("VolCurve1", volCurve); // Adding the vol curve to the market

    file.close();
}

void Market::updateMarketFromStockFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open stock price file: " << filePath << std::endl;
        return;
    }

    std::string line;
    while (getline(file, line)) {
        std::istringstream iss(line);
        std::string assetName;
        double price;
        char colon;

        if (!(iss >> assetName >> colon >> price)) {
            std::cerr << "Failed to parse line: " << line << std::endl;
            continue;  // Skip malformed lines
        }

        // Remove the colon from the asset name if it's present
        if (assetName.back() == ':') {
            assetName.pop_back();
        }

        addStockPrice(assetName, price);
    }
    file.close();
}

void Market::updateMarketFromCurveFile(const std::string& filePath, const std::string& curveName) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    RateCurve rateCurve(curveName);
    std::string line;
    while (getline(file, line)) {
        std::istringstream iss(line);
        std::string tenorStr;
        double rate;
        char delimiter;
        if (iss >> tenorStr >> delimiter >> rate && delimiter == ':') {
            rate /= 100.0;  // Convert percentage to decimal
            // Remove last character ('M' or 'Y') and calculate months
            int numMonths = std::stoi(tenorStr.substr(0, tenorStr.size() - 1));
            if (tenorStr.back() == 'Y') {
                numMonths *= 12;
            }
            Date tenorDate = this->asOf;  // Use the market's current date
            tenorDate.addMonths(numMonths);
            rateCurve.addRate(tenorDate, rate);
        }
    }
    this->addCurve(curveName, rateCurve);  // Use the existing method to add the curve to the market
    file.close();
}


double Market::getSpotPrice(const std::string &assetName) const {
    auto it = stockPrices.find(assetName);
    if (it != stockPrices.end()) {
        return it->second;
    }
    std::cerr << "Asset not found: " << assetName << ", returning default price 0." << std::endl;
    return 0;  // Return a default price or handle as needed
}

double Market::getVolatility(const std::string &assetName) const {
    auto it = vols.find(assetName);
    if (it != vols.end()) {
        return it->second.getLatestVol();
    }
    std::cerr << "Volatility data not found for asset: " << assetName << ", returning default vol 0." << std::endl;
    return 0;  // Return a default value or handle as needed
}


double Market::getRiskFreeRate() const {
    // Assuming risk-free rate is a single value for simplicity; implement
    // accordingly
    return riskFreeRate;
}

std::ostream &operator<<(std::ostream &os, const Market &mkt) {
    os << mkt.asOf << std::endl;
    return os;
}

std::istream &operator>>(std::istream &is, Market &mkt) {
    is >> mkt.asOf;
    return is;
}
