#include "Market.h"

using namespace std;

void RateCurve::display() const {
    cout << "rate curve:" << name << endl;
    for (size_t i=0; i<tenors.size(); i++) {
      cout << tenors[i] << ":" << rates[i] << endl;
  }
  cout << endl;
}

void RateCurve::addRate(Date tenor, double rate) {
  //consider to check if tenor already exist
  if (true){
    tenors.push_back(tenor);
    rates.push_back(rate);
  }  
}

double RateCurve::getRate(Date tenor) const {
  //use linear interpolation to get rate
  return 0;

}

void VolCurve::display() const {

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
  /*
  add display for bond price and stock price
  
  */
}

void Market::addCurve(const std::string& curveName, const RateCurve& curve){
  curves.emplace(curveName, curve);
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
