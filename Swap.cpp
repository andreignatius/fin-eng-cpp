#include <cmath>
#include "Market.h"
#include "Date.h"
#include "Swap.h"

// Implement getAnnuity method
double Swap::getAnnuity(const RateCurve& rates) const {
    time_t t = time(0);
    tm now_;
    localtime_s(&now_, &t);
    Date today;
    today.year = now_.tm_year + 1900;
    today.month = now_.tm_mon + 1;
    today.day = now_.tm_mday;
    //cout << today << endl;
    try {
        //cout << frequency << endl;

        vector<Date> paymentDates;
        Date nextPay = startDate;
        while (operator-(maturityDate, nextPay) >= 0) {
            //cout << "current examing pay Date: " << nextPay << endl;
            if (operator-(nextPay, today) > 0) {
                paymentDates.push_back(nextPay);
            }
            tm time = {};
            time.tm_year = nextPay.year - 1900;
            time.tm_mon = nextPay.month - 1 + 12 / frequency;
            time.tm_mday = nextPay.day;
            mktime(&time);

            nextPay = Date(time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);

            //cout << paymentDates.size() << endl;
        }

        double df_fix = 0;
        double df_flt = 0;

        for (size_t i = 0; i < paymentDates.size(); ++i) {
            double r = rates.getRate(paymentDates[i]);
            double T = operator-(paymentDates[i], today);
            df_flt = exp(-r * T);
            df_fix += df_flt;
        }

        return notional * df_fix / frequency;
    }
    catch (const std::bad_alloc& e) {
        std::cerr << "Out of memory space: " << e.what() << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception occurred." << std::endl;
    }

    return 0.0;

}


double Swap::getMktRate(const RateCurve& rates) const
{
    time_t t = time(0);
    tm now_;
    localtime_s(&now_, &t);
    Date today;
    today.year = now_.tm_year + 1900;
    today.month = now_.tm_mon + 1;
    today.day = now_.tm_mday;
    //cout << today << endl;
    try {
        //cout << frequency << endl;

        vector<Date> paymentDates;
        Date nextPay = startDate;
        while (operator-(maturityDate, nextPay) >= 0) {
            //cout << "current examing pay Date: " << nextPay << endl;
            if (operator-(nextPay, today) > 0) {
            paymentDates.push_back(nextPay);
            }
            tm time = {};
            time.tm_year = nextPay.year - 1900;
            time.tm_mon = nextPay.month - 1 + 12 / frequency;
            time.tm_mday = nextPay.day;
            mktime(&time);

            nextPay =  Date(time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);

        //cout << paymentDates.size() << endl;
        }

        double df_fix = 0;
        double df_flt = 0;

        for (size_t i = 0; i < paymentDates.size(); ++i) {
            double r = rates.getRate(paymentDates[i]);
            double T = operator-(paymentDates[i], today);
            df_flt = exp(-r * T);
            df_fix += df_flt;
        }
        cout << "current market rate: " << (1 - df_flt) / df_fix / frequency << endl;
        return (1 - df_flt) / df_fix / frequency;
    }
    catch (const std::bad_alloc& e) {
        std::cerr << "Out of memory space: " << e.what() << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception occurred." << std::endl;
    }

    return 0.0;
}



// Override the Payoff method with one parameter
double Swap::Payoff(double marketRate) const {
    cout << "I'm using the wrong pricer for swap" << endl;
    double annuity = 0;
    return annuity * (marketRate - tradeRate);
}

// Additional Payoff method with two parameters
double Swap::Payoff(double marketRate, double annuity) const {

    cout << "I'm using the RIGHT pricer for swap" << endl;
    if (!valid) {
        return 0;
    }
    return annuity * (marketRate - tradeRate);
}