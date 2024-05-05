#include "Swap.h"
#include <cmath>
#include <stdexcept>

double Swap::Payoff(double marketPrice) const {
    double annuity = getAnnuity();  // Use internal market data
    double currentRate = 0.0;
    try {
        double currentRate = market.getCurve("InterestRateCurve").getRate(startDate);
    } catch (const std::out_of_range& e) {
        // throw std::runtime_error("InterestRateCurve not found in market data.");
        std::cerr << "InterestRateCurve not found in market data. - using default rate 0." << std::endl;
        // Optionally use a fallback rate here
    }
    return annuity * (tradeRate - currentRate);
}

double Swap::getAnnuity() const {
    double totalAnnuity = 0.0;
    Date paymentDate = startDate;
    
    // Using differenceInDays to calculate the total number of periods
    long daysBetween = maturityDate.differenceInDays(startDate);
    double yearsBetween = static_cast<double>(daysBetween) / 365.25;  // Convert days to years
    int numPeriods = static_cast<int>(yearsBetween * frequency);  // Calculate the total number of periods

    std::cout << "maturityDate: " << maturityDate << std::endl;
    std::cout << "startDate: " << startDate << std::endl;
    std::cout << "numPeriods: " << numPeriods << std::endl;
    std::cout << "frequency: " << frequency << std::endl;

    for (int i = 1; i <= numPeriods; ++i) {
        paymentDate.addMonths(static_cast<int>(12 / frequency));  // Adjust the payment date according to the frequency
        double yearsSinceStart = static_cast<double>(paymentDate.differenceInDays(startDate)) / 365.25; // Convert days to years

        try {
            double rate = market.getCurve("InterestRateCurve").getRate(paymentDate);
            double discountFactor = exp(-rate * yearsSinceStart);
            totalAnnuity += notional * discountFactor;
        } catch (const std::out_of_range& e) {
            std::cerr << "Failed to find rate for date: " << paymentDate << " - using default rate 0." << std::endl;
            // Optionally use a fallback rate here
        }
    }
    return totalAnnuity;
}
