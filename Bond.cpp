#include "Bond.h"

double Bond::Payoff(double marketPrice) const {
	std::cout << "BondPrice: " << marketPrice << std::endl;
	double priceDifference = (marketPrice - tradePrice) / 100;  // Adjusted for bond pricing
	double payoff = priceDifference * notional;
	std::cout << "Notional :" << notional<<std::endl; 
	std::cout<< "TradePrice: " << tradePrice<<std::endl;
	std::cout<< "MarketPrice: " << marketPrice << std::endl;
	std::cout << "Payoff: " << payoff << std::endl;
	return payoff;
}
