# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)

# Project's name
project(MyProject)

# Set C++17 as the standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include Eigen
include_directories(${CMAKE_SOURCE_DIR}/eigen-3.4.0)

# Add executable
add_executable(main main.cpp AmericanTrade.cpp Bond.cpp BlackScholesPricer.cpp Date.cpp EuropeanTrade.cpp Logger.cpp Market.cpp JSONReader.cpp Pricer.cpp Swap.cpp CSVReader.cpp TenorMap.cpp Trade.cpp Utils.cpp RiskPortfolio.cpp)

# Optionally, you can set the output directory for binaries
set_target_properties(main PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
