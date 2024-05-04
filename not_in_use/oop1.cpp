#include <fstream>

#include "Market.h"
#include "TreePricer.h"
#include "EuropeanTrade.h"

int main()
{
  std::ifstream fin("simpleMkt.dat");
  if (fin) {
    Market mkt;
    //    fin >> mkt;
    fin >> mkt;
    mkt.Print();
    EuropeanOption callOption1(Call, 90, datetime_t(2016, 1, 1));
    EuropeanOption callOption2(Call, 91, datetime_t(2016, 1, 1));
    EuroCallSpread callSpread(90, 91, datetime_t(2016, 1, 1));
    std::cout << binomialTree(mkt, callOption1, 100) << std::endl;
    std::cout << binomialTree(mkt, callOption2, 100) << std::endl;
    std::cout << binomialTree(mkt, callSpread, 100) << std::endl;
    std::cout << binomialTree(mkt, callOption1, 100) - binomialTree(mkt, callOption2, 100) << std::endl;
    return 0;
  }    
  else {
    std::cerr << "no simpleMkt.dat found" << std::endl;
    return 1;
  }
}
