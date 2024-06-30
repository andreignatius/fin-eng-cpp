#ifndef MARKET_H
#define MARKET_H

#include "CSVReader.h"
#include "Date.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "TenorMap.h"
#include <cmath>
#include "Constants.h"
#include "Types.h"

using namespace std;

class RateCurve {
  public:
    Date startDate;
    RateCurve() {};
    RateCurve(const string &_name, const Date& _startDate) : name(_name), startDate(_startDate) {};
    void addRate(const Date& tenor, double rate);
    double getRate(Date tenor) const; // implement this function using linear interpolation
    std::vector<double>& getRates();
    void display() const;

  private:
    std::string name;
    vector<Date> tenors;
    vector<double> rates;
};

class VolCurve { // atm vol curve without smile
  public:
    Date startDate;
    VolCurve() {}
    VolCurve(const string &_name, const Date& _startDate) : name(_name), startDate(_startDate) {};
    void addVol(Date tenor, double volInDecimal); // implement this
    double getVol(const Date& tenor) const; // implement using linear interpolation
    vector<double> getVols() const;
    double getLatestVol() const; // Method to get the latest volatility
    void display() const;        // implement this

  private:
    string name;
    vector<Date> tenors;
    vector<double> vols;
};

class Market {
  public:
    Date asOf;
    Market(const Date &now) : asOf(now) {}
    void Print() const;
    void addCurve(const std::string &curveName,
                  const RateCurve &curve); // implement this
    void addVolCurve(const std::string &curveName,
                     const VolCurve &curve); // implement this
    void addBondPrice(const std::string &bondName,
                      double price); // implement this
    // void addStockPrice(const std::string &bondName,
    //                    double price); // implement this
    void addVolCurve(const std::string &stockName,
                     double price); // implement this
    
    void setRiskFreeRate(double rate);

    void addAssetType(const std::string &assetName, AssetType type);

    std::string dateToTenor(const Date &startDate, const Date &endDate) const;

    void updateMarketFromVolFile(const std::string &filePath, const std::string& curveName, const Date &specificDate); // Add this method
    void updateMarketFromBondFile(const std::string& filePath); // Method to load bond prices from a file
    void updateMarketFromStockFile(const std::string& filePath, const Date &specificDate);  // Method to load stock prices from a file
    void updateMarketFromCurveFile(const std::string& filePath, const std::string& curveName, const Date &specificDate);
    
    double getPriceOrRate(const std::string &assetName, const Date &specificDate) const;
    double getCurveRate(const std::string &assetName, const Date &specificDate) const;
    double getSpotPrice(const std::string &assetName, const Date &specificDate) const;
    double getBondPrice(const std::string &assetName) const;
    double getVolatility(const std::string &assetName) const;
    double getRiskFreeRate() const; // Assuming a single risk-free rate for simplicity
    AssetType getAssetType(const std::string &assetName) const;
    std::string assetTypeToString(AssetType type) const;

    void adjustInterestRates(double delta);
    void adjustVolatility(const std::string& underlying, double delta);

    RateCurve getCurve(const Date& date, const string& name) const;
    VolCurve getVolCurve(const Date& date, const string& name) const;

  private:
    unordered_map<string, VolCurve> volCurves;
    unordered_map<string, RateCurve> rateCurves;
    unordered_map<Date, unordered_map<string, RateCurve>, DateHash> dailyCurves; // Stores curves by date
    unordered_map<Date, unordered_map<string, VolCurve>, DateHash> dailyVolCurves; // Stores volatility curves by date
    unordered_map<string, double> bondPrices;
    unordered_map<Date, unordered_map<string, double>, DateHash> dailyStockPrices; // Stores daily stock prices by date
    double riskFreeRate = Constants::RISK_FREE_RATE;
    std::unordered_map<std::string, AssetType> assetTypes;
};

std::ostream &operator<<(std::ostream &os, const Market &obj);
std::istream &operator>>(std::istream &is, Market &obj);

#endif
