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

using namespace std;

class RateCurve {
  public:
    Date startDate;
    RateCurve() {};
    RateCurve(const string &_name, const Date& _startDate) : name(_name), startDate(_startDate) {};
    void addRate(const Date& tenor, double rate);
    double getRate(
        Date tenor) const; // implement this function using linear interpolation
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
    void addVol(Date tenor, double rate); // implement this
    double getVol(
        Date tenor) const; // implement this function using linear interpolation
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
    void addStockPrice(const std::string &bondName,
                       double price); // implement this
    void addVolCurve(const std::string &stockName,
                     double price); // implement this
    
    void setRiskFreeRate(double rate);

    void updateMarketFromVolFile(const std::string &filePath, const std::string& curveName); // Add this method
    void updateMarketFromBondFile(const std::string& filePath); // Method to load bond prices from a file
    void updateMarketFromStockFile(const std::string& filePath);  // Method to load stock prices from a file
    void updateMarketFromCurveFile(const std::string& filePath, const std::string& curveName);
    
    double getSpotPrice(const std::string &assetName) const;
    double getBondPrice(const std::string &assetName) const;
    double getVolatility(const std::string &assetName) const;
    double getRiskFreeRate() const; // Assuming a single risk-free rate for simplicity

    // inline RateCurve getCurve(const string& name) { return curves[name]; };
    inline const RateCurve &getCurve(const string &name) const {
        return curves.at(name);
    };
    inline VolCurve getVolCurve(const string &name) { return vols[name]; };

  private:
    unordered_map<string, VolCurve> vols;
    unordered_map<string, RateCurve> curves;
    unordered_map<string, double> bondPrices;
    unordered_map<string, double> stockPrices;
    double riskFreeRate = 0.02;
};

std::ostream &operator<<(std::ostream &os, const Market &obj);
std::istream &operator>>(std::istream &is, Market &obj);

#endif
