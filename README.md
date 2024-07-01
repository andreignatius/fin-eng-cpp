# Financial Engineering Project using C++

## Overview
This is a C++ project that:
  1. Computes and analyze the price of Bonds, Swaps, European Options and American options given relevant market information and contract details (such as SOFR curve, volatility, contract expiry date, frequency etc)
  2. Create either a risk minimizing investment portfolio or a PnL maximizing investment portfolio

### Part 1
1. Create market data objects using the .txt files containing stock price, interest rate and volatility
2. Create a portfolio of bonds, swaps, European options, American options
3. Create a pricer and price each of the trade
4. Analyze and compare:
   1. CRR binomial tree vs Black model for an European option
   2. American option vs European option for CRR binomial tree model

Example:
```
// user input
mkt.updateMarketFromVolFile((DATA_PATH / "vol_euro.csv").string(), "EuropeanOption");
myPortfolio.push_back(new EuropeanOption(Call, 700, Date(2025, 12, 31), "AAPL"));

// console output
Trade: EuropeanOption, Underlying: AAPL
CRR tree priced trade with PV: 93.3583

// log file output
[INFO] Processing European Option. Underlying= AAPL, Type= 1, Strike= 700.000000, Expiry= 2025-12-31
[INFO] trade: EuropeanOption AAPL PV : 105.594224
```


### Part 2
This part of the project continues from Part 1, but with additional requirements:
1. Create a portfolio of trades using trade info given in attached portfolio.txt.
2. Create 2 days market data using market data attached.
3. Price the portfolio and output the PV and Risk (DV01 and Vega) for 2 dates.
   1. Output the pv and risk result for each date into file.
   2. Compute the PnL of each trade using PV difference between 2 dates
4. Analysing the Greeks of the portfolio and design a small strategy that either:
   1. Square off the risk of portfolio
   2. Maximise portfolio PnL given a fixed risk limit of Dv01 and Vega

Example:
```
//user input
pnl = [200253, -135622, -28646.2, -275254, -297739, -214078, 18.1616, 159.34, 112.997, 0.0110558, 5.79535e-12, 15.2482, 126.244, 143.147, 0.00756741]
dv01 = [-0.02903, -0.03290, 0.00140, -0.04719, -0.01726, 0.02154, 0.01352, -0.00362, -0.03104, -0.03160, 0.04825, 0.02215, -0.02379, -0.00126, -0.02079]
vega = [3.82344, 4.09414, 1.98718, 2.66876, 1.70367, 2.70116, 3.23691, 4.13602, 0.74357, 1.15589, 4.64055, 3.43809, 2.02648, 0.97007, 4.85105]
dv01_limit = 0.05
vega_limit = 10

//console output
Optimal solution found.
[1.         0.00100001 0.00100003 0.001      0.001      0.001
 0.00100675 0.58168899 0.99999033 0.0010275  0.00100542 0.00100636
 0.99997891 0.9999921  0.00100522]
Total DV01 of the portfolio: -0.08726826602093173
Total Vega of the portfolio: 9.999983194641016
Total PnL of the portfolio: 199776.75946649472
```

## Project Features
### Implementation aspects
- Cross-OS collaboration using CMake
- Text Data Parsing: Efficiently reads and processes input data from CSV files.
- CSV Data Parsing: Efficiently reads and processes input data from CSV files.
- JSON Data Parsing: Efficiently reads and processes input data from JSON files.
- Object Oriented Programming, such as classes and structures for
  - different market data (e.g. historical volatility, SOFR)
  - different financial instruments (e.g. Options, Swaps and Bonds)
  - different pricing models (e.g. Black-scholes model and CRR Binomial tree model for options)
- Error Handling: Robust error handling for data processing and pricing calculations.
- Logging: output runtime results and warnings in a log file for review
- Version control: using GitHub and following industry best practice (e.g. branch out, pull request, review, merge)
- Python integration **(Part 2)**
  - creating and optimizing appropriate portfolio in python
  - input and output of python are in .csv
- Various other C++ techniques, including but not limited to:
  - Polymorphism
  - Operator overload
  - Pointers
  - Virtual functions and abstract base class
  - fstream
  - iterators
  - dynamic casting
  - smart pointers **(Part 2)**
 
### Financial aspects
- Bond pricing
- Swap pricing
- Option pricing using Binomial tree model
- Option pricing using Black model
- Interest rate sensitivity computation - DV01 **(Part 2)**
- Volatility sensitivity computation - Vega **(Part 2)**
- Risk adjusted portfolio optimization **(Part 2)**

## User Instruction
### System Prerequisites
- C++17 or later
- CMake 3.10 or later
- Any standard C++ compiler (e.g., GCC, Clang, MSVC)
- Mac or Windows OS

### Building and running the project
1. Prepare market data in .csv or .txt format. Formatting should follow the provided sample files in /data directory. For **(Part 2)**, use /market_data directory.
2. Prepare your portfolio using .json file. Formatting should follow the provided sample files in /data directory
3. Adjust filenames accordingly in main.cpp inside main() function
4. Compile and run using either of the following scripts
  - **For windows OS, run in console: `./build_and_run.bat`**
  - **For Mac OS, run in console: `./build_and_run.sh`**

## Notable Assumptions
1. Bond Pricing
- Bond value is a matter of difference between market rate and the price we entered the position. 
- We are not pricing the bond from discounting cashflows

2. Swap Pricing
- Time between maturity and start date is annualized using the following formula: (Maturity Date - Start Date)/ 365.25
- Start date refers to the start date of the swap
- Assume no forward start
- Linear interpolation of discount factor, where the rate curve is assumed to be zero coupons rates
- Fixed rate given is annual rate,
- FixedLegPV = annuity * (fixedRate / frequency)
- FloatLegPV = notional * (1 - DF_last), where DF_last refers to the discount factor at the time of the final cashflow exchange
- If rate data is missing, fallback rate is 0

3. Option Pricing
- Time between maturity and start date is annualized using the following formula: (Maturity Date - Start Date)/ 365.25
- Start date is assumed to be today's date when the program is run
- European options in the portfolio is priced with the TreePricer
- Risk free rate fix at 2%



## Code Structure
### Source Code Files
- **main.cpp**: Entry point of the application.
- **Logger.cpp**: Implementation of the logging utility.
- **JSONReader.cpp**: Handles reading and parsing JSON data.
- **CSVReader.cpp**: Handles reading CSV files.
- **AmericanTrade.cpp**: Implementation of American trade-related functionality.
- **EuropeanTrade.cpp**: Implementation of European trade-related functionality.
- **BlackScholesPricer.cpp**: Implementation of the Black-Scholes pricer.
- **Bond.cpp**: Implementation of bond-related functionality.
- **Date.cpp**: Implementation of date handling functionality.
- **Market.cpp**: Implementation of market information functionality.
- **PortfolioMaker.cpp**: Implementation of portfolio construction functionality. **(Part 2)**
- **Pricer.cpp**: General pricing functionality.
- **RiskEnginee.cpp**: Implementation of DV01 and Vega calculation. **(Part 2)**
- **Swap.cpp**: Implementation of swap-related functionality.
- **TenorMap.cpp**: Implementation of tenor mapping.
- **Trade.cpp**: General trade functionality.
- **Types.h**: Common types used across the project.

### Header Files
- **Logger.h**: Header file for the logging utility.
- **JSONReader.h**: Header file for JSON reader.
- **CSVReader.h**: Header file for CSV reader.
- **AmericanTrade.h**: Header file for American trade-related functionality.
- **EuropeanTrade.h**: Header file for European trade-related functionality.
- **BlackScholesPricer.h**: Header file for Black-Scholes pricer.
- **Bond.h**: Header file for bond-related functionality.
- **Date.h**: Header file for date handling functionality.
- **Market.h**: Header file for market information functionality.
- **Payoff.h**: Header file for payoff-related functionality.
- **PortfolioMaker.h**: Header file for creating a portfolio of trades. **(Part 2)**
- **Pricer.h**: Header file for pricing functionality.
- **RiskEngine.h**: Header file for DV01 and Vega calculation functionality. **(Part 2)**
- **Swap.h**: Header file for swap-related functionality.
- **TenorMap.h**: Header file for tenor mapping.
- **Trade.h**: Header file for trade functionality.
- **Types.h**: Header file for common types.

### Python Files
- **portfolio_optimization.py**: Creating a portfolio that minimizes the DV01 and Vega, or maximizing PnL for a given risk limit **(Part 2)**

### Directories
- **data**: contains the .txt or .csv or .json file, where market information is stored at
- **output**: contains the log files produced by the logger feature
- **market_data**: contains the .txt or .csv file for **(Part 2)**
