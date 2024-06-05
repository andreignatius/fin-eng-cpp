# Financial Engineering Project using C++

## Overview
This project involves pricing various financial instruments such as options and swaps using C++. 

The project takes in market data in .txt or .csv or .json format, as well as user-defined financial instruments to perform these operations.

## Features
- CSV Data Parsing: Efficiently reads and processes input data from CSV files.
- JSON Data Parsing: Efficiently reads and processes input data from JSON files.
- Object Oriented Programming, includes classes and structures for
  - different market data (e.g. historical volatility, SOFR)
  - different financial instruments (e.g. Options, Swaps and Bonds)
  - different pricing models (e.g. Black-scholes model and CRR Binomial tree model for options)
- Error Handling: Robust error handling for data processing and pricing calculations.
- Cross-OS collaboration using CMake
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

## Building and running the project
- To build and run using windows OS, run : `./build_and_run.bat`
- To build and run using Mac OS, run : `./build_and_run.sh`

## Example
```
// user input
mkt.updateMarketFromVolFile((DATA_PATH / "vol_euro.csv").string(), "EuropeanOption");
myPortfolio.push_back(new EuropeanOption(Call, 700, Date(2025, 12, 31), "AAPL"));

// console output
Trade: EuropeanOption, Underlying: AAPL
CRR tree priced trade with PV: 93.3583
```

## Code Structure
- asdf
- asdf
- asdf
