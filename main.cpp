#include <fstream>
#include <ctime>
#include <chrono>

#include "Market.h"
#include "Pricer.h"
#include "EuropeanTrade.h"
#include "Bond.h"
#include "Swap.h"
#include "AmericanTrade.h"

using namespace std;

void readFromFile(const string& fileName, string& outPut){
  string lineText;
  ifstream MyReadFile(fileName);
  while (getline (MyReadFile, lineText)) {
    outPut.append(lineText);
  }
  MyReadFile.close();
}

int main()
{
  //task 1, create an market data object, and update the market data from from txt file 
  std::time_t t = std::time(0);
  auto now_ = std::localtime(&t);
  Date valueDate;
  valueDate.year = now_->tm_year + 1900;
  valueDate.month = now_->tm_mon + 1;
  valueDate.year = now_->tm_mday;

  Market mkt = Market(valueDate);
  /*
  load data from file and update market object with data
  */

  //task 2, create a portfolio of bond, swap, european option, american option
  //for each time, at least should have long / short, different tenor or expiry, different underlying
  //totally no less than 16 trades
  vector<Trade*> myPortfolio;
  Trade* bond = new Bond(Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 103.5);
  myPortfolio.push_back(bond);


  //task 3, creat a pricer and price the portfolio, output the pricing result of each deal.
  Pricer* treePricer = new CRRBinomialTreePricer(10);
  for (auto trade: myPortfolio) {
    double pv = treePricer->Price(mkt, trade);
    //log pv details out in a file

  }

  //task 4, analyzing pricing result
  // a) compare CRR binomial tree result for an european option vs Black model
  // b) compare CRR binomial tree result for an american option vs european option

  //final
  cout << "Project build successfully!" << endl;
  return 0;

}
