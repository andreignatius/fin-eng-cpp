#include <fstream>

#include "Market.h"
#include "TreePricerNonVirtual.h"
#include "EuropeanTradeNonVirtual.h"

int main()
{
  std::ifstream fin("simpleMkt.dat");
  if (fin) {
    Market mkt;
    fin >> mkt;
    mkt.Print();
    try
      {
	EuropeanOption callOption1(Call, 90, Date(2016, 1, 1));
	std::cout << binomialTree(mkt, callOption1, 100) << std::endl;
      }
    catch ( const std::exception & ex ) {
      std::cerr << ex.what() << std::endl;
    }
    return 0;
  }    
  else {
    std::cerr << "no simpleMkt.dat found" << std::endl;
    return 1;
  }
}
