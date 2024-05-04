#include <fstream>
#include "Market.h"
int main() {
  std::ifstream fin("simpleMkt.dat");
  if (fin) {
    Market mkt;
    fin >> mkt;
    mkt.Print();
  } else {
    std::cerr << "simpleMkt.dat not accessible" << std::endl;
  }
  return 0;
}
