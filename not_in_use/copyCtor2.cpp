#include <fstream>
#include "MarketTemp.h"
int main() {
  std::ifstream fin("simpleMkt.dat");
  if (fin) {
    Market mkt;
    fin >> mkt;
    std::cout << "Market1--------" << std::endl;
    mkt.pp = new datetime_t;
    mkt.pp->year = 2010;
    mkt.pp->month = 1;
    mkt.pp->day = 1;
    mkt.Print();
    
    Market mkt2 = mkt;
    std::cout << "Market2--------" << std::endl;    
    mkt2.Print();
    std::cout << mkt2.pp->year << std::endl;

    Market mkt3;
    std::cout << mkt3.pp << std::endl;
    mkt3 = mkt2;
    std::cout << mkt3.pp << std::endl;
    mkt3 = mkt3;
    std::cout << mkt3.pp << std::endl;
    std::cout << "Market3--------" << std::endl;
    mkt3.Print();
    std::cout << mkt3.pp->year << std::endl;
  }
  else {
    std::cerr << "simpleMkt.dat not accessible" << std::endl;
  }
  return 0;
}
