#include "Bond.h"

double Bond::Payoff(double marketPrice) const {
	std::cout << "Bond underlying: " << underlying << std::endl;
	std::cout << "Bond price: " << marketPrice << std::endl;
	double priceDifference = (marketPrice - tradePrice) / 100;  // Adjusted for bond pricing
	double payoff = priceDifference * notional;
	std::cout << "Notional :" << notional << ", Trade price: " << tradePrice << ", Market price: " << marketPrice << std::endl;
	std::cout << "Bond payoff: " << payoff << std::endl;
	return payoff;
}
