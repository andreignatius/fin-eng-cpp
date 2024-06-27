#include "Bond.h"
#include "Factory.h"
double Bond::Payoff(double marketPrice) const {
	std::cout << "*bondPrice: " << marketPrice << std::endl;
	double priceDifference = (marketPrice - tradePrice) / 100;  // Adjusted for bond pricing
	double payoff = priceDifference * notional;
	std::cout << "notional :" << notional << " tradePrice: " << tradePrice << " marketPrice: " << marketPrice << std::endl;
	std::cout << "@compute payoff:" << payoff << std::endl;
	return payoff;
}
