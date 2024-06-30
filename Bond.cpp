#include "Bond.h"
#include <sstream>

double Bond::Payoff(double marketPrice) const {
    double couponAmount; // Use internal market data
    double currentRate = 0.0;
    double rate;
    double pv = 0;
    double yearsSinceStart;
    double fixedLegPV;
    double floatLegPV;
    double DF_last;
    int paymentInMonths;
    int paymentCount;
    Date paymentDate = startDate;
    Date seed = maturityDate;
    vector<Date> paymentSchedule;
    // 1. get coupon amount
    couponAmount = notional * couponRate * frequency;

    // 2. generate the bond schedule; CANNOT USE FUNCTION BECAUSE const keyword
    /*
        PAYMENT SCHEDULE GENERATION START
    */
    paymentInMonths = static_cast<int>(12 * frequency);
    while (seed > startDate) {
        paymentSchedule.push_back(seed);
        seed.subtractMonths(paymentInMonths);
    }
    paymentSchedule.push_back(startDate);
    if (paymentSchedule.size() < 2)
        throw std::runtime_error("Error: invalid schedule, check input!");
    std::reverse(paymentSchedule.begin(), paymentSchedule.end());
    /*
        PAYMENT SCHEDULE GENERATION END
    */

    // 3. now do the calculation

    // 4. Follow Swap a bit for PV calc
    long daysBetween = maturityDate.differenceInDays(startDate);
    double yearsBetween = static_cast<double>(daysBetween) /
                          Constants::NUM_DAYS_IN_YEAR; // Convert days to years
    int numPeriods = paymentSchedule.size();
    std::cout << "numPeriods : " << numPeriods << " ; startDate : " << startDate
              << " ; maturityDate : " << maturityDate
              << " ; Notional : " << notional
              << " ; Coupon Amt : " << couponAmount << " ; Freq : " << frequency
              << std::endl;
    for (int i = 0; i < numPeriods; ++i) {
        if (paymentSchedule[i] <= valueDate) {
            // coupon already disbursed
            std::cout << paymentSchedule[i] << " < " << valueDate
                      << " ; NOT INCLUDED IN BOND PV CALC" << std::endl;
        } else {
            // the rest
            yearsSinceStart =
                static_cast<double>(
                    paymentSchedule[i].differenceInDays(valueDate)) /
                Constants::NUM_DAYS_IN_YEAR; // Convert days to years
            rate = market.getCurve(valueDate, curveName)
                       .getRate(paymentSchedule[i]);
            double discountFactor = exp(-rate * yearsSinceStart);
            if (i == paymentCount - 1) {
                // on maturity coupon + notional
                pv += discountFactor * (couponAmount + notional);
                /*
                std::cout << "COUNT FINAL " << i
                          << " DATE : " << paymentSchedule[i] << " PV : " << pv
                          << std::endl;
                          */
            } else {
                // only coupon
                pv += discountFactor * couponAmount;
                /*
                std::cout << "COUNT " << i << " DATE : " << paymentSchedule[i]
                          << " PV : " << pv << std::endl;
                          */
            }
        }
    }
    return pv;
}

std::string Bond::toString() const {
    std::ostringstream oss;
    oss << "Type: " << getType() << ", Start Date: " << startDate.toString()
        << ", End Date: " << maturityDate.toString() << ", Notional: $"
        << notional << ", Trade Price: $" << tradePrice
        << ", Coupon Rate: " << (couponRate * 100) << "%"
        << ", CurveName: " << curveName << ", UUID: " << uuid;
    return oss.str();
}