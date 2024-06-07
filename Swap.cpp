#include "Swap.h"
#include <cmath>
#include <stdexcept>
#include <sstream>

double Swap::Payoff(double marketPrice) const { // TODO marketPrice is redundant
    double annuity = getAnnuity(); // Use internal market data
    double currentRate = 0.0;
    double rate;
    double pv;
    double yearsSinceStart;
    Date paymentDate = startDate;
    double fixedLegPV;
    double floatLegPV;
    double DF_last;

    fixedLegPV = annuity * (fixedRate / frequency); // assume fixedRate is annual rate

    // find last discount rate for floating leg pv calculation
    // TODO may need clean up and checks
    try {
        currentRate = market.getCurve(curveName).getRate(startDate);
        std::cout<<"current rate is "<<currentRate<<std::endl;
    } catch (const std::out_of_range &e) {
        std::cerr << "specified curve not found in market data. - using "
                     "default rate 0."
                  << std::endl;
    }
    long daysBetween    = maturityDate.differenceInDays(startDate);
    double yearsBetween = static_cast<double>(daysBetween) / 365.25; // Convert days to years
    int numPeriods      = static_cast<int>(yearsBetween * frequency); // Calculate the total number of periods
    for (int i = 1; i <= numPeriods; ++i) {
        paymentDate.addMonths(static_cast<int>(
            12 /
            frequency)); // Adjust the payment date according to the frequency
        yearsSinceStart =
            static_cast<double>(paymentDate.differenceInDays(startDate)) /
            365.25; // Convert days to years
        rate = market.getCurve(curveName).getRate(paymentDate);
        double discountFactor = exp(-rate * yearsSinceStart);
    }

    DF_last = exp(-rate * yearsSinceStart);
    floatLegPV = notional * (1 - DF_last);
    std::cout<<isFixedForFloating<<std::endl;
    if (isFixedForFloating) {
        pv = floatLegPV - fixedLegPV;
    } else {
        pv = fixedLegPV - floatLegPV;
    }
    // std::cout << "fix PV: " << fixedLegPV << ", float PV:
    // "<<floatLegPV<<std::endl;
    return pv;
}

std::string Swap::toString() const {
    std::ostringstream oss;
    oss << "Type: " << getType() << ", Start Date: " << startDate.toString()
        << ", End Date: " << maturityDate.toString() << ", Notional: $" << notional
        << ", Underlying: " << curveName << ", UUID: " << uuid;
    return oss.str();
}

// annuity = DV01*notional
double Swap::getAnnuity() const {
    double annuity = 0.0;
    Date paymentDate = startDate;

    // Using differenceInDays to calculate the total number of periods
    long daysBetween = maturityDate.differenceInDays(startDate);
    double yearsBetween =
        static_cast<double>(daysBetween) / 365.25; // Convert days to years
    int numPeriods = static_cast<int>(
        yearsBetween * frequency); // Calculate the total number of periods

    std::cout << "maturityDate: " << maturityDate << std::endl;
    std::cout << "startDate: " << startDate << std::endl;
    std::cout << "numPeriods: " << numPeriods << std::endl;
    std::cout << "frequency: " << frequency << std::endl;

    for (int i = 1; i <= numPeriods; ++i) {
        paymentDate.addMonths(static_cast<int>(
            12 /
            frequency)); // Adjust the payment date according to the frequency
        double yearsSinceStart =
            static_cast<double>(paymentDate.differenceInDays(startDate)) /
            365.25; // Convert days to years
        double disc_rate = 0.0;
        try {
            disc_rate = market.getCurve(curveName).getRate(paymentDate);
        } catch (const std::out_of_range &e) {
            // Handle error appropriately, e.g., use a fallback rate
            std::cerr << "Failed to find rate for date: " << paymentDate
                      << ". Using default rate 0." << std::endl;
        }
        double discountFactor = exp(-disc_rate * yearsSinceStart);
        annuity += notional * discountFactor;
    }
    return annuity;
}
