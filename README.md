# Financial Engineering Project using C++

## Overview
This is a C++ project that:
  1. Computes and analyze the price of Bonds, Swaps, European Options and American options given relevant market information and contract details (such as SOFR curve, volatility, contract expiry date, payout frequency etc)
  2. Create either a risk minimizing investment portfolio or a PnL maximizing investment portfolio

### Part 1
1. Create market data objects using the .txt files containing stock price, interest rate and volatility
2. Create a portfolio of bonds, swaps, European options, American options
3. Create a pricer and price each of the trade
4. Analyze and compare:
   1. CRR binomial tree vs Black model for an European option
   2. American option vs European option for CRR binomial tree model

Illustrative example:
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

Illustrative example:
```
//user input
pnl = np.array([100, 120, 90, 130, 110, 140])
dv01_vectors = np.array([
    [0.1, -0.2, 0.15, 0.1],
    [0.2, 0.1, -0.25, 0.3],
    [0.3, -0.2, 0.1, 0.25],
    [-0.25, 0.35, 0.2, -0.3],
    [0.15, -0.25, 0.3, 0.1],
    [0.2, 0.15, -0.35, 0.25]
])
vega_vectors = np.array([
    [0.05, 0.2, -0.1, 0.15],
    [-0.15, 0.1, 0.2, -0.25],
    [0.2, -0.15, 0.1, 0.2],
    [0.25, 0.3, -0.2, 0.1],
    [0.1, -0.25, 0.3, -0.2],
    [0.2, 0.1, -0.25, 0.3]
])
dv01_min = -2.0
dv01_max = 2.0
vega_min = -1.5
vega_max = 1.5

//console output
Optimal Weights: [0.         5.71428568 0.         4.60317457 0.         0.        ]
Portfolio PnL: 1284.126976425897
Portfolio dv01: [-0.00793651  2.18253967 -0.50793651  0.33333333]
Portfolio Vega: [ 0.29365079  1.95238094  0.22222222 -0.96825396]
```

## Project Features
### Implementation aspects
- Cross-OS (Windows & Mac) collaboration using CMake
- Building project using batch file
- Text Data Parsing: Efficiently reads and processes input data from CSV files.
- CSV Data Parsing: Efficiently reads and processes input data from CSV files.
- JSON Data Parsing: Efficiently reads and processes input data from JSON files.
- JSON Output: Storing the computation results into JSON for easy reference. **(Part 2)**
- Object Oriented Programming, such as classes and structures for
  - different market data (e.g. historical volatility, SOFR)
  - different financial instruments (e.g. Options, Swaps and Bonds)
  - different pricing models (e.g. Black-scholes model and CRR Binomial tree model for options)
- Error Handling: Robust error handling for data processing and pricing calculations.
- Logging: output runtime results and warnings in a log file for review
- Version control: using GitHub and following industry best practice (e.g. branch out, pull request, review, merge)
- Python integration **(Part 2)**
  - run the python script as part of build process
  - creating and optimizing appropriate portfolio in python
  - Read the JSON file containing DV01 and Vega and PnL
  - Install Numpy and SciPy package of specific versions onto user's environment
- Various other C++ techniques, including but not limited to:
  - Polymorphism
  - Operator overload
  - Pointers
  - Virtual functions and abstract base class
  - fstream
  - iterators
  - dynamic casting
  - smart pointers **(Part 2)**
  - multi threading (near-complete) **(Part 2)**
 
### Financial aspects
- Bond pricing
- Swap pricing
- Option pricing using Binomial tree model
- Option pricing using Black model
- Interest rate non-parallel shift sensitivity computation - DV01 vector for each instrument **(Part 2)**
- Volatility non-parallel shift sensitivity computation - Vega vector for each instrument **(Part 2)**
- Risk adjusted portfolio optimization **(Part 2)**
- Compute instrument inter-day change in PV **(Part 2)**

## User Instruction
### System Prerequisites
- C++17 or later
- CMake 3.10 or later
- Any standard C++ compiler (e.g., GCC, Clang, MSVC)
- Mac or Windows OS

### Building and running the project
1. Prepare market data in .csv or .txt format. Formatting should follow the provided sample files in /data directory. For **(Part 2)**, use /market_data directory.
2. Prepare your portfolio using .json file. Formatting should follow the provided sample files in /data directory. For **(Part 2)**, use /market_data directory.
3. Adjust filenames accordingly in main.cpp inside main() function
4. Compile and run using either of the following scripts
  - **For windows OS, run in console: `./build_and_run.bat`**
  - **For Mac OS, run in console: `./build_and_run.sh`**

### Project Output
- For Log files, see /output directory for .txt files
- For Risk computations, see /output directory for .json files.

## Notable Assumptions
### Bond Pricing
- Bond value is a matter of difference between market rate and the price we entered the position. 
- We are not pricing the bond from discounting cashflows

### Swap Pricing
- Time between maturity and start date is annualized using the following formula: (Maturity Date - Start Date)/ 365.25
- Start date refers to the start date of the swap
- Assume no forward start
- Linear interpolation of discount factor, where the rate curve is assumed to be zero coupons rates
- Fixed rate given is annual rate,
- FixedLegPV = annuity * (fixedRate / frequency)
- FloatLegPV = notional * (1 - DF_last), where DF_last refers to the discount factor at the time of the final cashflow exchange
- If rate data is missing, fallback rate is 0

### Option Pricing
- Time between maturity and start date is annualized using the following formula: (Maturity Date - Start Date)/ 365.25
- Start date is assumed to be today's date when the program is run
- European options in the portfolio is priced with the TreePricer
- Risk free rate fix at 2%

### Risk analysis
- Assumed non-parallel shift in interest rate and volatility, shocking each tenor individually **(Part 2)**
- Assumed instrument weight between 0 and 10 (no selling) **(Part 2)**
- DV01 and Vega constraint is considered satisfied as long as the sum of DV01 (or Vega) across tenor falls within the bound **(Part 2)**
- Scaling can be performed before optimization to prevent ValueError: Positive directional derivative for linesearch

## Code Structure
### Source Code Files
- **main.cpp**: Entry point of the application.
- **AmericanTrade.cpp**: Implementation of American trade-related functionality.
- **BlackScholesPricer.cpp**: Implementation of the Black-Scholes pricer.
- **Bond.cpp**: Implementation of bond-related functionality.
- **CSVReader.cpp**: Handles reading CSV files.
- **Date.cpp**: Implementation of date handling functionality.
- **EuropeanTrade.cpp**: Implementation of European trade-related functionality.
- **JSONReader.cpp**: Handles reading and parsing JSON data.
- **Logger.cpp**: Implementation of the logging utility.
- **Market.cpp**: Implementation of market information functionality.
- **PortfolioMaker.cpp**: Implementation of portfolio construction functionality. **(Part 2)**
- **Pricer.cpp**: General pricing functionality.
- **RiskEnginee.cpp**: Implementation of DV01 and Vega calculation. **(Part 2)**
- **Swap.cpp**: Implementation of swap-related functionality.
- **TenorMap.cpp**: Implementation of tenor mapping.
- **Trade.cpp**: General trade functionality.
- **Types.cpp**: Common types used across the project.
- **Utils.cpp**: Header file for helper functions such as converting Option type to string. **(Part 2)**

### Header Files
- **AmericanTrade.h**: Header file for American trade-related functionality.
- **BlackScholesPricer.h**: Header file for Black-Scholes pricer.
- **Bond.h**: Header file for bond-related functionality.
- **Constants.h**: Header file for otherwise magic numbers, such as yield curve shock size, number of days in year. **(Part 2)**
- **CSVReader.h**: Header file for CSV reader.
- **Date.h**: Header file for date handling functionality.
- **EuropeanTrade.h**: Header file for European trade-related functionality.
- **JSONReader.h**: Header file for JSON reader.
- **Logger.h**: Header file for the logging utility.
- **Market.h**: Header file for market information functionality.
- **Payoff.h**: Header file for payoff-related functionality.
- **PortfolioMaker.h**: Header file for creating a portfolio of trades. **(Part 2)**
- **Pricer.h**: Header file for pricing functionality.
- **RiskEngine.h**: Header file for DV01 and Vega calculation functionality. **(Part 2)**
- **Swap.h**: Header file for swap-related functionality.
- **TenorMap.h**: Header file for tenor mapping.
- **Trade.h**: Header file for trade functionality.
- **Types.h**: Header file for common types.
- **Utils.h**: Header file for helper functions such as converting Option type to string. **(Part 2)**

### Python related files
- **portfolio_optimization.py**: Creating a portfolio that minimizes the DV01 and Vega, or maximizing PnL for a given risk limit. Automatically make use of the latest JSON file. **(Part 2)**
- **requirements.txt**: specify the numpy and scipy version used in the above python script. Will be automatically installed by `./build_and_run.bat` if not present. **(Part 2)**

### Directories
- **data**: contains the .txt or .csv or .json file, where market information is stored at
- **output**: contains the log files produced by the logger feature
- **market_data**: contains the .txt or .csv file for **(Part 2)**
