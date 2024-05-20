#include <fstream>
#include <ctime>
#include <chrono>

#include <vector>
#include <string>
#include <sstream>
#include <map>

#include "Market.h"
#include "Pricer.h"
#include "EuropeanTrade.h"
#include "Bond.h"
#include "Swap.h"
#include "AmericanTrade.h"

using namespace std;

// Black-Scholes analytic pricer

// Function to calculate the cumulative normal distribution
double cnorm(double x) {
    return (1.0 + erf(x / sqrt(2.0))) / 2.0;
}

// Black-Scholes formula for different payoff types
double bsPrice(double S, double r, double vol, double T, double strike, int optType) {
    double fwd = S * exp(r * T);
    double stdev = vol * sqrt(T);
    double d1 = log(fwd / strike) / stdev + stdev / 2;
    double d2 = d1 - stdev;

    switch (optType) {
    case 0:
        return exp(-r * T) * (fwd * cnorm(d1) - cnorm(d2) * strike);
    case 1:
        return exp(-r * T) * (strike * cnorm(-d2) - cnorm(-d1) * fwd);
    case 2:
        return exp(-r * T) * cnorm(d2);
    case 3:
        return exp(-r * T) * (cnorm(-d2));
    default:
        throw invalid_argument("Not supported payoff type");
    }
}


vector<string> splitString(const string& str, char delimiter) {
    vector<string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }

    result.push_back(str.substr(start));
    return result;
}

// Function to parse the rates and return a vector of rates
vector<double> parseRates(const string& input, RateCurve& rateCurve) {
    vector<double> rates;

    // Find the position of "ON" and extract the name part
    size_t pos = input.find("ON");
    if (pos == string::npos) {
        cerr << "Error: The input string does not contain 'ON'." << endl;
        return rates;  // Return empty vector
    }

    string name = input.substr(0, pos);
    //cout << "Curve Name: " << name << endl;
    rateCurve = RateCurve(name);
    // Split the remaining part by ':'
    string ratesPart = input.substr(pos);
    vector<string> ratePairs = splitString(ratesPart, ':');

    // Process each rate pair
    for (const string& pair : ratePairs) {
        // Split each pair by space to separate tenor and rate
        size_t spacePos = pair.find(' ');
        if (spacePos == string::npos) continue;  // Skip invalid pairs

        string rateStr = pair.substr(spacePos + 1);

        // Convert rate to double
        double rate = stod(rateStr.substr(0, rateStr.size() - 1));  // Remove the '%' and convert to double

        // Add rate to the vector
        rates.push_back(rate/100);
    }

    return rates;
}
string slice(const std::string& str, size_t start, size_t end) {
    if (start >= str.size() || start > end) {
        return ""; // Return an empty string if the indices are invalid
    }
    return str.substr(start, end - start);
}

// Function to add days to a Date object
Date addDays(const Date& date, int days) {
    tm time = {};
    time.tm_year = date.year - 1900;
    time.tm_mon = date.month - 1;
    time.tm_mday = date.day + days;
    mktime(&time);

    return Date(time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);
}

// Function to add months to a Date object
Date addMonths(const Date& date, int months) {
    tm time = {};
    time.tm_year = date.year - 1900;
    time.tm_mon = date.month - 1 + months;
    time.tm_mday = date.day;
    mktime(&time);

    return Date(time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);
}

// Function to add years to a Date object
Date addYears(const Date& date, int years) {
    tm time = {};
    time.tm_year = date.year - 1900 + years;
    time.tm_mon = date.month - 1;
    time.tm_mday = date.day;
    mktime(&time);

    return Date(time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);
}

// Function to parse the vols and return a vector of vols

vector<double> parseVols(const string& input) {
    vector<double> vols;

    // Split the input string by ':'
    stringstream ss(input);
    string pair;
    while (getline(ss, pair, ':')) {
        // Find the position of '%'
        size_t pos = pair.find('%');
        if (pos == string::npos) {
            //cerr << "Error: Invalid vol format in pair: " << pair << endl;
            continue; // Skip invalid pairs
        }

        // Extract the vol substring between ":" and "%"
        string volStr = pair.substr(pair.find(':') + 1, pos - pair.find(':') - 1);

        // Convert vol to double and add to the vector
        double vol = stod(volStr);
        vols.push_back(vol/100);
    }

    return vols;
}

pair<string, string> extractDecimalAndName(const string& str) {
    string decimalPart;
    string namePart;

    bool firstDot = true;
    bool foundNonDigit = false;

    for (char ch : str) {
        if ((isdigit(ch) || (ch == '.' && firstDot)) && !foundNonDigit) {
            decimalPart += ch;
            if (ch == '.') {
                firstDot = false;  // Set firstDot to false after encountering the first dot
            }
        }
        else {
            foundNonDigit = true;
            namePart += ch;
        }
    }

    return make_pair(decimalPart, namePart);
}


string removeSpaces(const string& str) {
    string result = str;
    result.erase(remove_if(result.begin(), result.end(), [](unsigned char c) { return std::isspace(c); }), result.end());
    return result;
}

map<string, double> parsePrices(const string& input) {
    map<string, double> assets; // Map to store names and prices
    vector<string> substrings = splitString(input, ':');

    vector<double> spots;
    vector<string> names;


    // Process each substring to extract asset names and prices
    string name; // Variable to store asset name
    if (substrings.size() < 2) {
        cout << "Invalid input!" << endl;
        return assets;
    }
    names.push_back(removeSpaces(substrings[0]));
    //cout << substrings[0] << endl;
    for (size_t i = 1; i < substrings.size(); ++i) {
        auto result = extractDecimalAndName(removeSpaces(substrings[i]));
        if (!result.first.empty()) {
            spots.push_back(stod(result.first));
            //cout << stod(result.first) << endl;
        }
        if (!result.second.empty()) {
            names.push_back(result.second);
            //cout << result.second << endl;
        }
    }
    
    for (size_t i = 0; i < names.size(); ++i) {
        assets[names[i]] = spots[i];
    }

    return assets; // Return the populated map
}


void readFromFile(const string& fileName, string& outPut){
  string lineText;
  ifstream MyReadFile(fileName);

  if (!MyReadFile.is_open()) {
      cerr << "Error: Unable to open file " << fileName << endl;
      return;
  }
  while (getline (MyReadFile, lineText)) {
   // cout << "Read line: " << lineText << endl;
    outPut.append(lineText);
  }
  MyReadFile.close();
}

int main()
{

    //task 1, create an market data object, and update the market data from from txt file 


    time_t t = time(0);
    tm now_;
    localtime_s(&now_, &t);
    Date valueDate;
    valueDate.year = now_.tm_year + 1900;
    valueDate.month = now_.tm_mon + 1;
    valueDate.day = now_.tm_mday;
    //cout << "value Date is: " << valueDate.year << "-" << valueDate.month << "-" << valueDate.day << endl;

    Market mkt = Market(valueDate);
    /*
    load data from file and update market object with data
    */

    // Read data from the file and update the Market object
    // Create vector of tenor dates
    vector<Date> tenors;

    tenors.push_back(addDays(valueDate, 1));   // 1 day later
    tenors.push_back(addMonths(valueDate, 3));   // 3 months later
    tenors.push_back(addMonths(valueDate, 6));   // 6 months later
    tenors.push_back(addMonths(valueDate, 9));   // 9 months later
    tenors.push_back(addYears(valueDate, 1));    // 1 year later
    tenors.push_back(addYears(valueDate, 2));    // 2 years later
    tenors.push_back(addYears(valueDate, 5));    // 5 years later
    tenors.push_back(addYears(valueDate, 10));   // 10 years later
    //// Print the tenor dates
    //cout << "Tenor Dates:" << endl;
    //for (const Date& date : tenors) {
    //    cout << date.year << "-" << date.month << "-" << date.day << endl;
    //}

    RateCurve rateC;
    string filename = "curve.txt";
    string fileContents;

    readFromFile(filename, fileContents);

    vector<double> rates = parseRates(fileContents, rateC);

    //// Print out the rates
    //cout << "Parsed Rates:" << endl;
    //for (double rate : rates) {
    //    cout << rate << " in floating number" << endl;
    //}
    // Check if the sizes are equal
    if (rates.size() == tenors.size()) {
        // Loop through each tenor and rate
        for (size_t i = 0; i < tenors.size(); ++i) {
            rateC.addRate(tenors[i], rates[i]);
        }
    }
    else {
        std::cout << "The rates vector and the tenors vector do not have the same number of items." << std::endl;
    }
    //rateC.display();
    mkt.addCurve(rateC.getName(), rateC);

    //cout << "time differences are: " << endl;
    //cout << operator-(Date(2034, 5, 19), Date(2024, 5, 20)) << endl;
    //cout << operator-(Date(2034, 5, 20), Date(2024, 5, 20)) << endl;

    cout << "getRate test cases: " << endl;
    try {
        cout << "Rate of 2024-07-19: " << rateC.getRate(Date(2024, 7, 19)) << endl;
        //cout << rateC.getRate(Date(2034, 5, 19)) << endl;
        //cout << rateC.getRate(Date(2044, 7, 19)) << endl;

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



    tenors[0] = addMonths(valueDate, 1);   // 1 month later
    VolCurve volC("volCurve");
    filename = "vol.txt";
    //cout << "vol file name:\n" << filename << endl;
    fileContents = "";
    readFromFile(filename, fileContents);
    //cout << "vol fileContents:\n" << fileContents << endl;
    vector<double> vols = parseVols(fileContents);

    //// Print out the parsed rates
    //for (double vol : vols) {
    //    cout << vol << endl;
    //}

    if (vols.size() == tenors.size()) {
        // Loop through each tenor and rate
        for (size_t i = 0; i < tenors.size(); ++i) {
            volC.addVol(tenors[i], vols[i]);
        }
    }
    else {
        std::cout << "The rates vector and the tenors vector do not have the same number of items." << std::endl;
    }
    //volC.display();   
    mkt.addVolCurve(volC.getName(), volC);



    filename = "bondPrice.txt";
    //cout << "vol file name:\n" << filename << endl;
    fileContents = "";
    readFromFile(filename, fileContents);
    //cout << "bondPrice fileContents:\n" << fileContents << endl;
    // Parse the bond string and populate the bond map
    map<string, double> bonds = parsePrices(fileContents);

    // Print the bond map

    for (const auto& pair : bonds) {
        //cout << removeSpaces(pair.first) << " -> " << pair.second << endl;
        mkt.addBondPrice(pair.first, pair.second);
    }

    filename = "stockPrice.txt";
    fileContents = "";
    readFromFile(filename, fileContents);
    //cout << "stockPrice fileContents:\n" << fileContents << endl;
    map<string, double> stocks = parsePrices(fileContents);
    // Print the stock map

    for (const auto& pair : stocks) {
        //cout << removeSpaces(pair.first) << " -> " << pair.second << endl;
        mkt.addStockPrice(pair.first, pair.second);
    }

    mkt.Print();


  //task 2, create a portfolio of bond, swap, european option, american option
  //for each time, at least should have long / short, different tenor or expiry, different underlying
  //totally no less than 16 trades
  vector<Trade*> myPortfolio;
  Trade* bond1 = new Bond("SGD-GOV", Date(2024, 1, 1), Date(2034, 1, 1), 10000000, 103.5);
  myPortfolio.push_back(bond1);

  Trade* bond2 = new Bond("USD-GOV", Date(2024, 1, 1), Date(2034, 1, 1), -10000000, 103.5);
  myPortfolio.push_back(bond2);

  Trade* bond3 = new Bond("NoDataBond", Date(2024, 1, 1), Date(2034, 1, 1), -10000000, 103.5);
  myPortfolio.push_back(bond3);

  Trade* swap1 = new Swap("USD-SOFR", Date(2024, 1, 1), Date(2026, 1, 1), 1000000, 0.05, 2); // Semi-annual frequency
  myPortfolio.push_back(swap1);

  Trade* swap2 = new Swap("NoSwap", Date(2024, 1, 1), Date(2028, 1, 1), -2000000, 0.03, 4); // Quarterly frequency
  myPortfolio.push_back(swap2);

  Trade* swap3 = new Swap("wrongfrequency", Date(2024, 1, 1), Date(2028, 1, 1), 2000000, 0.03, 5); // wrong frequency
  myPortfolio.push_back(swap3);

  Trade* Euro1 = new EuropeanOption("APPL", Call, 690, Date(2024, 7, 20)); // European call option
  myPortfolio.push_back(Euro1);

  Trade* Amer1 = new AmericanOption("APPL", Call, 690, Date(2024, 7, 19)); // American call option
  myPortfolio.push_back(Amer1);


  Trade* Euro2 = new EuropeanOption("SP500", Put, 5000, Date(2024, 7, 19)); // European put option
  myPortfolio.push_back(Euro2);

  Trade* Amer2 = new AmericanOption("SP500", Put, 5000, Date(2024, 7, 19)); // American put option
  myPortfolio.push_back(Amer2);

  Trade* Euro3 = new EuropeanOption("STI", BinaryCall, 3500, Date(2024, 9, 19)); // European binary call option
  myPortfolio.push_back(Euro3);

  Trade* Amer3 = new AmericanOption("STI", BinaryCall, 3500, Date(2024, 9, 19)); // American  binary call option
  myPortfolio.push_back(Amer3);

  Trade* Euro4 = new EuropeanOption("STI", BinaryPut, 3300, Date(2024, 9, 19)); // European binary put option
  myPortfolio.push_back(Euro4);

  Trade* Amer4 = new AmericanOption("STI", BinaryPut, 3300, Date(2024, 9, 19)); // American binary put option
  myPortfolio.push_back(Amer4);

  Trade* Euro5 = new EuroCallSpread("SP500", 5600, 6090, Date(2024, 10, 19)); // European call spread
  myPortfolio.push_back(Euro5);

  Trade* Amer5 = new AmerCallSpread("SP500", 5600, 6090, Date(2024, 10, 19)); // American call spread
  myPortfolio.push_back(Amer5);

  Trade* Euro6 = new EuropeanOption("NoName", Call, 690, Date(2024, 8, 19)); // European call option - NoName error
  myPortfolio.push_back(Euro6);


  Trade* Amer6 = new AmerCallSpread("NoName2", 5600, 6090, Date(2024, 10, 19)); // American call spread - NoName error
  myPortfolio.push_back(Amer6);



  //task 3, creat a pricer and price the portfolio, output the pricing result of each deal.
  Pricer* treePricer = new CRRBinomialTreePricer(300);
  Pricer* generalPricer = new Pricer();
  //Pricer* bsPricer = new BlackScholesPricer();
  //log pv details out in a file 
  ofstream outFile("PortfolioValueLog.txt");
  for (auto trade: myPortfolio) {
    outFile << "#" << endl;
    string tradeType = trade->getType();
    if (tradeType == "BondTrade") {

        Bond* bondPtr = dynamic_cast<Bond*>(trade);
        if (bondPtr) {
            double pv = generalPricer->Price(mkt, trade);
            cout << "Bond Holdings: " << bondPtr->getBondName() << ", Present Value: " << pv << endl;
            //log pv details out in a file 
            outFile << "Bond Holdings: " << bondPtr->getBondName() << ", Present Value: " << pv << "#" << endl;
        }
    }
    else if (tradeType == "SwapTrade") {

        //cout << "I'll call general pricer to price swap" << endl;
        Swap* swapPtr = dynamic_cast<Swap*>(trade);
        if (swapPtr) {
            double pv = generalPricer->Price(mkt, trade);
            cout << "Swap Holdings: " << swapPtr->getCurveName() << " swap, Present Value: " << pv << endl;
            //log pv details out in a file 
            outFile << "Swap Holdings: " << swapPtr->getCurveName() << ", Present Value: " << pv << "#" << endl;
        }

    }
    else if (tradeType == "TreeProduct") {

        //cout << "I'll call tree pricer to price tree product" << endl;
        TreeProduct* treePtr = dynamic_cast<TreeProduct*>(trade);
        if (treePtr) {

            //cout << "tree pricer in action!" << endl;

            // Assuming 'trade' is a pointer to a Trade object
            EuropeanOption* euroOption = dynamic_cast<EuropeanOption*>(trade);
            EuroCallSpread* euroSpread = dynamic_cast<EuroCallSpread*>(trade);
            AmericanOption* amerOption = dynamic_cast<AmericanOption*>(trade);
            AmerCallSpread* amerSpread = dynamic_cast<AmerCallSpread*>(trade);
            if (euroSpread) {
                // 'trade' is an instance of EuroCallSpread
                Date expiry = euroSpread->getExpiryDate();
                double k1 = euroSpread->getStrike1();
                double k2 = euroSpread->getStrike2();
                string name = euroSpread->getAssetName();
                cout << "extracted info: " << name << ", "
                    << k1 << ", " << k2 << ", " << expiry << endl;
                Trade* EuroCall1 = new EuropeanOption(name, Call, k1, expiry);
                Trade* EuroCall2 = new EuropeanOption(name, Call, k2, expiry);
                double pv1 = treePricer->Price(mkt, EuroCall1);
               
                double pv2 = treePricer->Price(mkt, EuroCall2);
                double pv = pv1 - pv2;
                cout << "EuroCall1: " << euroSpread->getAssetName() << ", Present Value: " << pv1 << endl;
                cout << "EuroCall2: " << euroSpread->getAssetName() << ", Present Value: " << pv2 << endl;
                cout << "EuroCallSpread: " << euroSpread->getAssetName() << ", Present Value: " << pv << endl;
                outFile << "EuroCallSpread: " << euroSpread->getAssetName() << ", Present Value: " << pv << "#" << endl;
            }
            else if (euroOption) {

                //task 4, analyzing pricing result

//// a) compare CRR binomial tree result for an european option vs Black model




                RateCurve mkt_rates = mkt.getCurve("USD-SOFR");
                VolCurve mkt_vols = mkt.getVolCurve("volCurve");
                //mkt_rates.display();
                //mkt_vols.display();
                const auto& mkt_stocks = mkt.getAllStockPrices();
                // 'trade' is an instance of EuroOption
                Date expiry = euroOption->getExpiryDate();
                double k = euroOption->getStrike();
                int OptTp = euroOption->getOptionType();
                string name = euroOption->getAssetName();
                double r = mkt_rates.getRate(expiry);
                double vol = mkt_vols.getVol(expiry);
                double S0 = 0, pv_bs = 0;
                double T = expiry - mkt.asOf;
                auto it = mkt_stocks.find(name);
                if (it != mkt_stocks.end()) {
                    S0 = it->second;
                    //cout << "I've found the current market price: " << stockPrice << endl;
                }
                else {
                    //std::cerr << "Warning: Stock price not found for asset: " << name << ". Defaulting to 0." << std::endl;
                    pv_bs = 0;
                }
                cout << "extracted info: Asset=" << name << ", expiry="
                    << expiry << ", strike=" << k << ", OptType=" << OptTp 
                    << ", r=" << r << ", vol=" << vol << ", T=" << T << endl;
                cout << "Option Value by Black-Scholes is " << bsPrice(S0, r, vol, T, k, OptTp) << endl;
                double pv = treePricer->Price(mkt, trade);
                cout << "EuroOption " << euroOption->getOptionType() <<
                    ": " << euroOption->getAssetName() << ", Present Value: " << pv << endl;
                outFile << "EuroOption " << euroOption->getOptionType() <<
                    ": " << euroOption->getAssetName() << ", Present Value: " << pv << "#" << endl;

                outFile << "extracted info: Asset=" << name << ", expiry="
                    << expiry << ", strike=" << k << ", OptType=" << OptTp
                    << ", r=" << r << ", vol=" << vol << ", T=" << T << "#" << endl;
                outFile << "Option Value by Black-Scholes is " << bsPrice(S0, r, vol, T, k, OptTp) << "#" << endl;
            }
            else if (amerSpread) {
                // 'trade' is an instance of EuroCallSpread
                Date expiry = amerSpread->getExpiryDate();
                double k1 = amerSpread->getStrike1();
                double k2 = amerSpread->getStrike2();
                string name = amerSpread->getAssetName();
                cout << "extracted info: " << name << ", "
                    << k1 << ", " << k2 << ", " << expiry << endl;
                Trade* AmerCall1 = new AmericanOption(name, Call, k1, expiry);
                Trade* AmerCall2 = new AmericanOption(name, Call, k2, expiry);
                double pv1 = treePricer->Price(mkt, AmerCall1);

                double pv2 = treePricer->Price(mkt, AmerCall2);
                double pv = pv1 - pv2;
                cout << "AmerCall1: " << amerSpread->getAssetName() << ", Present Value: " << pv1 << endl;
                cout << "AmerCall2: " << amerSpread->getAssetName() << ", Present Value: " << pv2 << endl;
                cout << "AmerCallSpread: " << amerSpread->getAssetName() << ", Present Value: " << pv << endl;
                outFile << "AmerCallSpread: " << amerSpread->getAssetName() << ", Present Value: " << pv << "#" << endl;



            }
            else if (amerOption) {

                // b) compare CRR binomial tree result for an american option vs european option


                double pv = treePricer->Price(mkt, trade);
                cout << "AmericanOption " << amerOption->getOptionType() <<
                    ": " << amerOption->getAssetName() << ", Present Value: " << pv << endl;
                outFile << "AmericanOption " << amerOption->getOptionType() <<
                    ": " << amerOption->getAssetName() << ", Present Value: " << pv << "#" << endl;

            }

            else {

                double pv = 0;
                cout << "Not supported tree Product" << endl;
            }


            //cout << "Swap Holdings: " << treePtr->getAssetName() << " swap, Present Value: " << pv << endl;
            //log pv details out in a file 
            //outFile << "Swap Holdings: " << swapPtr->getCurveName() << ", Present Value: " << pv << "#" << endl;
        }

    }


  }





  outFile.close();






  //task 4, analyzing pricing result
 
  //// a) compare CRR binomial tree result for an european option vs Black model





  // b) compare CRR binomial tree result for an american option vs european option

  //final
  cout << "Project build successfully!" << endl;

  system("pause");
  return 0;

}
