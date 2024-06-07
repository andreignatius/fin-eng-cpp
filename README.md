# Financial Engineering Project using C++

## Overview
This project involves pricing various financial instruments such as options and swaps using C++. 

The project takes in market data in .txt or .csv or .json format, as well as user-defined financial instruments to perform these operations.

## Features
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
- Various C++ techniques, including but not limited to:
  - Polymorphism
  - Operator overload
  - Pointers
  - Virtual functions and abstract base class
  - fstream
  - iterators

## Prerequisites
- C++17 or later
- CMake 3.10 or later
- Any standard C++ compiler (e.g., GCC, Clang, MSVC)

## Instruction for building and running the project
- For windows OS, run : `./build_and_run.bat`
- For Mac OS, run : `./build_and_run.sh`

## Example
A short extract of user input and project output
```
// user input
mkt.updateMarketFromVolFile((DATA_PATH / "vol_euro.csv").string(), "EuropeanOption");
myPortfolio.push_back(new EuropeanOption(Call, 700, Date(2025, 12, 31), "AAPL"));

// console output
Trade: EuropeanOption, Underlying: AAPL
CRR tree priced trade with PV: 93.3583

// log file output
[INFO] Processing European Option. Underlying= AAPL, Type= 1, Strike= 700.000000, Expiry= 2025-12-31
[INFO] *****European Option Price*****: 93.3583
```

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
- **Pricer.cpp**: General pricing functionality.
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
- **Pricer.h**: Header file for pricing functionality.
- **Swap.h**: Header file for swap-related functionality.
- **TenorMap.h**: Header file for tenor mapping.
- **Trade.h**: Header file for trade functionality.
- **Types.h**: Header file for common types.

### Directories
- **data**: contains the .txt or .csv or .json file, where market information is stored at
- **output**: contains the log files produced by the logger feature
