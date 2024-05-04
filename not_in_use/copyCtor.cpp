#include <fstream>
#include "Market.h"
int main() {
  std::ifstream fin("simpleMkt.dat");
  if (fin) {
    Market mkt;
    fin >> mkt;
    mkt.Print();
    Market mkt2 = mkt;
    mkt2.Print();
  }
  else {
    std::cerr << "simpleMkt.dat not accessible" << std::endl;
  }
  return 0;
}
