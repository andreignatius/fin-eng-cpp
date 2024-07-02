#include "RiskEngine.h"

void RiskEngine::computeRisk(string riskType, Trade *trade, Date valueDate,
                             Pricer *pricer) {
    string type = trade->getType();
    std::cout << "RISK CALC FOR : " << type << std::endl;
    auto *americanOption = dynamic_cast<AmericanOption *>(trade);
    auto *europeanOption = dynamic_cast<EuropeanOption *>(trade);
    auto *bond = dynamic_cast<Bond *>(trade);
    auto *swap = dynamic_cast<Swap *>(trade);

    double totalDv01 = 0, vega = 0, pv = 0;


    if (riskType == "dv01") {
        /*
            The logic is:
            1. get the asset name
            2. if rate --> shock tenor one by one
            3. if stock --> shock it immediately
        */
        string underlying;
        double rate;
        double pv_up;
        double pv_down;
        if (type == "BondTrade" || type == "SwapTrade") {
            // 1. get the underlying curve
            // underlying = bond->getUnderlying();
            underlying = (bond ? bond->getUnderlying() : swap->getUnderlying());
        
            RateCurve theCurve = theMarket.getCurve(valueDate, underlying);
            std::vector<Date> tenors = theCurve.getTenors();
            // 2. shock the curve + do the PV
            //      1. Copy the curve first
            RateCurve upCurve = theCurve;
            RateCurve downCurve = theCurve;
            //      2. Iter through the keys of the curve
            for (auto it = tenors.begin(); it != tenors.end(); ++it) {
                double currRate = theCurve.getRate(*it);
                upCurve.addRate(*it, currRate + 0.0001);
                downCurve.addRate(*it, currRate - 0.0001);
                //  3. NOW PRICE THIS
                // pv_up = bond->PayoffCurve(upCurve);
                // pv_down = bond->PayoffCurve(downCurve);

                pv_up = (bond ? bond->PayoffCurve(upCurve) : swap->PayoffCurve(upCurve));
                pv_down = (bond ? bond->PayoffCurve(downCurve) : swap->PayoffCurve(downCurve));

                double dv01 = (pv_up - pv_down) / 2.0;
                totalDv01 += dv01;
                if (bond) {
                    std::cout << "BOND DV01 " << *it << " = " << dv01 << std::endl;
                } else {
                    std::cout << "SWAP DV01 " << *it << " = " << dv01 << std::endl;
                }
                
            }
            if (bond) {
                std::cout << "FINAL BOND DV01: " << totalDv01 << std::endl;
            } else {
                std::cout << "FINAL SWAP DV01: " << totalDv01 << std::endl;
            }
        } 
        else if (americanOption) {
            std::cout << "americanOption dv01 calc" << std::endl;
            double dv01 =
                americanOption->CalculateDV01(theMarket, valueDate, pricer);
            std::cout << "americanOption dv01  = " << dv01 << std::endl;
        } else if (europeanOption) {
            std::cout << "europeanOption dv01 calc" << std::endl;
            double dv01 =
                europeanOption->CalculateDV01(theMarket, valueDate, pricer);
            std::cout << "europeanOption dv01  = " << dv01 << std::endl;
        } else {
            std::cout << "UNRECOGNIZED INSTRUMENT" << std::endl;
        }
    }

    if (riskType == "vega") {
        /*
            The logic is:
            1. get the asset name
            2. if rate --> shock tenor one by one
            3. if stock --> shock it immediately
        */
        string underlying;
        double rate;
        double pv_up;
        double pv_down;
        if (americanOption) {
            std::cout << "americanOption vega calc" << std::endl;
            double dv01 =
                americanOption->CalculateVega(theMarket, valueDate, pricer);
            std::cout << "americanOption vega  = " << dv01 << std::endl;
        } else if (europeanOption) {
            std::cout << "europeanOption vega calc" << std::endl;
            double dv01 =
                europeanOption->CalculateVega(theMarket, valueDate, pricer);
            std::cout << "europeanOption vega  = " << dv01 << std::endl;
        } else {
            std::cout << "NO NEED VEGA CHECK" << std::endl;
        }
    }
    if (riskType == "price") {
        std::cout << "PRICE RISK" << std::endl;
        // to be added
    }
}
