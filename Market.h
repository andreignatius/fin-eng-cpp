#ifndef MARKET_H
#define MARKET_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include "Date.h"

#include <string>

using namespace std;

class RateCurve {
public:
  RateCurve(){};
  RateCurve(const string& _name): name(_name) {} ; 
  void addRate(Date tenor, double rate);
  double getRate(Date tenor) const; //implement this function using linear interpolation
  void display() const;
  const string& getName() const {
      return name;
  }
private:
  string name;
  vector<Date> tenors;
  vector<double> rates;
};

class VolCurve { // atm vol curve without smile
public:
  VolCurve(){}
  VolCurve(const string& _name): name(_name) {} ; 
  void addVol(Date tenor, double rate); //implement this
  double getVol(Date tenor) const; //implement this function using linear interpolation
  void display() const; //implement this
  const string& getName() const {
      return name;
  }
private:
  string name;
  vector<Date> tenors;
  vector<double> vols;
};

class Market
{
public:
  Date asOf;
  Market(const Date& now): asOf(now) {}
  void Print() const;
  void addCurve(const std::string& curveName, const RateCurve& curve);//implement this
  void addVolCurve(const std::string& curveName, const VolCurve& curve);//implement this
  void addBondPrice(const std::string& bondName, double price);//implement this
  void addStockPrice(const std::string& stockName, double price);//implement this
  

  RateCurve getCurve(const string& name) const;
  VolCurve getVolCurve(const string& name) const;

  //inline RateCurve getCurve(const string& name) const { return curves[name]; };
  //inline VolCurve getVolCurve(const string& name) const { return vols[name]; };

  const std::unordered_map<std::string, double>& getAllBondPrices() const {
      return bondPrices;
  }
  const std::unordered_map<std::string, double>& getAllStockPrices() const {
      return stockPrices;
  }
private:
  
  unordered_map<string, VolCurve> vols;
  unordered_map<string, RateCurve> curves;
  unordered_map<string, double> bondPrices;
  unordered_map<string, double> stockPrices;
};

std::ostream& operator<<(std::ostream& os, const Market& obj);
std::istream& operator>>(std::istream& is, Market& obj);

#endif
