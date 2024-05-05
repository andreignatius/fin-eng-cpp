#include "Market.h"
#include <algorithm>

using namespace std;

void RateCurve::display() const {
    cout << "rate curve:" << name << endl;
    for (size_t i=0; i<tenors.size(); i++) {
      cout << tenors[i] << ":" << rates[i] << endl;
  }
  cout << endl;
}

void RateCurve::addRate(Date tenor, double rate) {
  // consider to check if tenor already exist
  // Search for the tenor in the existing list of tenors
  auto it = std::find_if(tenors.begin(), tenors.end(), [&tenor](const Date& existingTenor) {
    return existingTenor == tenor; // Assuming you have an equality operator defined for Date
  });

  if (it != tenors.end()) {
    // Tenor already exists, replace the rate
    auto index = std::distance(tenors.begin(), it);
    rates[index] = rate;
    cout << "Updated existing tenor " << tenor << " with new rate " << rate << "." << endl;
  } else {
    // Tenor does not exist, add new tenor and rate
    tenors.push_back(tenor);
    rates.push_back(rate);
    cout << "Added new tenor " << tenor << " with rate " << rate << "." << endl;
  }
}

double RateCurve::getRate(Date tenor) const {
  if (tenors.empty()) return 0; // No rates added

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
  if (tenors.empty()) return 0; // No vols added

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

void VolCurve::display() const {
  cout << "Volatility curve for: " << name << endl;
  for (size_t i = 0; i < tenors.size(); i++) {
    cout << "Tenor: " << tenors[i] << " Vol: " << vols[i] << "%" << endl;
  }
}

void Market::Print() const
{
  cout << "market asof: " << asOf << endl;
  
  for (auto curve: curves) {
    curve.second.display();
  }
  for (auto vol: vols) {
    vol.second.display();
  }

  // Add display for bond price and stock price
  cout << "Bond Prices:" << endl;
  for (const auto& bond : bondPrices) {
    cout << "Bond: " << bond.first << " Price: " << bond.second << endl;
  }
  cout << "Stock Prices:" << endl;
  for (const auto& stock : stockPrices) {
    cout << "Stock: " << stock.first << " Price: " << stock.second << endl;
  }
}

void Market::addCurve(const std::string& curveName, const RateCurve& curve){
  curves.emplace(curveName, curve);
}

void Market::addVolCurve(const std::string& curveName, const VolCurve& curve) {
  vols[curveName] = curve;
}

void Market::addBondPrice(const std::string& bondName, double price) {
  bondPrices[bondName] = price;
}

void Market::addStockPrice(const std::string& stockName, double price) {
  stockPrices[stockName] = price;
}

std::ostream& operator<<(std::ostream& os, const Market& mkt)
{
  os << mkt.asOf << std::endl;
  return os;
}

std::istream& operator>>(std::istream& is, Market& mkt)
{
  is >> mkt.asOf;
  return is;
}
