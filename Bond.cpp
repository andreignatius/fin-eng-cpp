#include "Bond.h"
#include <sstream>

double Bond::Payoff(double marketPrice) const {
	std::cout << "*bondPrice: " << marketPrice << std::endl;
	double priceDifference = (marketPrice - tradePrice) / 100;  // Adjusted for bond pricing
	double payoff = priceDifference * notional;
	std::cout << "notional :" << notional << " tradePrice: " << tradePrice << " marketPrice: " << marketPrice << std::endl;
	std::cout << "@compute payoff:" << payoff << std::endl;
	return payoff;
}

std::string Bond::toString() const {
    std::ostringstream oss;
    oss << "Type: " << getType() << ", Start Date: " << startDate.toString()
        << ", End Date: " << maturityDate.toString() << ", Notional: $" << notional
        << ", Trade Price: $" << tradePrice << ", Coupon Rate: " << (couponRate * 100) << "%"
        << ", Underlying: " << underlying << ", UUID: " << uuid;
    return oss.str();
}