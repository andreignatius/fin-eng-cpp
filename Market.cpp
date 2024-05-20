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
    for (size_t i = 0; i < tenors.size(); ++i) {
        if (operator-(tenors[i], tenor) == 0) {
            // Tenor already exists
            return; // Exit the function without adding the tenor
        }
    }

   tenors.push_back(tenor);
   rates.push_back(rate);
    
}

double RateCurve::getRate(Date tenor) const {
  //use linear interpolation to get rate
    //cout << "tenors lenth is: " << tenors.size() << endl;
    //cout << "rate curve:" << name << endl;

    try {
        if (tenors.empty()) {
            throw runtime_error("RateCurve is empty.");
        }
        if (operator-(tenors[0], tenor) >= 0) {
            return rates[0]; // left boundry
        }
        else if (operator-(tenors[tenors.size()-1], tenor) <= 0) {
            return rates[rates.size() - 1]; // right boundry


        }
        // Linear interpolation
        for (size_t i = 0; i < tenors.size(); ++i) {

            if (operator-(tenors[i], tenor) >= 0) {

                double r1 = rates[i - 1];
                double r2 = rates[i];

                return r1 + (r2 - r1) * operator-(tenor, tenors[i - 1]) / operator-(tenors[i], tenors[i - 1]);
            }
        }
    }
    catch (const std::bad_alloc& e) {
        std::cerr << "Out of memory space: " << e.what() << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception occurred." << std::endl;
    }

    return 0.0;

}
void VolCurve::addVol(Date tenor, double vol) {
    //consider to check if tenor already exist
    for (size_t i = 0; i < tenors.size(); ++i) {
        if (operator-(tenors[i], tenor) == 0) {
            // Tenor already exists
            return; // Exit the function without adding the tenor
        }
    }

    tenors.push_back(tenor);
    vols.push_back(vol);

}
double VolCurve::getVol(Date tenor) const
{
    try {
        if (tenors.empty()) {
            throw runtime_error("RateCurve is empty.");
        }
        if (operator-(tenors[0], tenor) >= 0) {
            return vols[0]; // left boundry
        }
        else if (operator-(tenors[tenors.size() - 1], tenor) <= 0) {
            return vols[vols.size() - 1]; // right boundry


        }
        // Linear interpolation
        for (size_t i = 0; i < tenors.size(); ++i) {

            if (operator-(tenors[i], tenor) >= 0) {

                double v1 = vols[i - 1];
                double v2 = vols[i];

                return v1 + (v2 - v1) * operator-(tenor, tenors[i - 1]) / operator-(tenors[i], tenors[i - 1]);
            }
        }
    }
    catch (const std::bad_alloc& e) {
        std::cerr << "Out of memory space: " << e.what() << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception occurred." << std::endl;
    }

    return 0.0;
}
void VolCurve::display() const {
    cout << "vol curve:" << name << endl;

    for (size_t i = 0; i < tenors.size(); i++) {
            cout << tenors[i] << ":" << vols[i] << endl;
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
  /*
  add display for bond price and stock price
  */
  cout << "\n" << endl;
  cout << "Bond Prices: " << endl;

  for (const auto& pair : bondPrices) {
      cout << pair.first << " -> " << pair.second << endl;
  }

  cout << "\n" << endl;
  cout << "Stock Prices: " << endl;
  for (const auto& pair : stockPrices) {
      cout << pair.first << " -> " << pair.second << endl;
  }
 
}

void Market::addCurve(const std::string& curveName, const RateCurve& curve){
  curves.emplace(curveName, curve);
}

void Market::addVolCurve(const std::string& curveName, const VolCurve& vol) {
  vols.emplace(curveName, vol);
}
void Market::addBondPrice(const std::string& bondName, double price) {
    bondPrices[bondName] = price;
}

void Market::addStockPrice(const std::string& stockName, double price) {
    stockPrices[stockName] = price;
}



RateCurve Market::getCurve(const string& name) const {
    auto it = this->curves.find(name);
    if (it != this->curves.end()) {
        return it->second;
    }

    auto defaultIt = this->curves.find("USD-SOFR");
    if (defaultIt != this->curves.end()) {
        return defaultIt->second;
    }

    return RateCurve("Empty");
}

VolCurve Market::getVolCurve(const string& name) const {

    auto defaultIt = this->vols.find("volCurve");
    if (defaultIt != this->vols.end()) {
        return defaultIt->second;
    }
    return VolCurve("volCurve");
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
