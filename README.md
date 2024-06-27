FULL set of QF633 part2 project
developed in Windows Visual Studio
NOT TO DIRECT MERGE with main (which is from apple VS code)
Revision notes:
1. tree pricer for Amer options using step interest interpolated from curve
2. new pricer for BS analytical price, rather just raw functions
3. try to adopt on factory style of trade creation
4. calibration of tree pricer will first use a flat rate same as Euro options
5. Re-construct on pricer logics as according to lastest portoflio input format


# file adding list as per windows visual studio sequence:

Header files: `AmericanTrade.h` `Bond.h` `Date.h` `EuropeanTrade.h` `Market.h` `helper.h` `RiskEngin.h` `Payoff.h` `Pricer.h` `Swap.h` `Trade.h` `TreeProduct.h` `Types.h` `Factory.h`

Source files: `Date.cpp` `main.cpp` `Maket.cpp` `Pricer.cpp` `Swap.cpp` `Factory.cpp` `RiskEngin.cpp` `Bond.cpp`

Resource files: `portfolio.txt` `stock_price_20240601.txt` `stock_price_20240602.txt` `usd_sofr_20240601.txt` `usd_sofr_20240602.txt` `vol_20240601.txt` `vol_20240602.txt`

output files: `PortfolioValueLog2.txt`

other files: `QF633 Binomial Tree Validation.ipynb` `_QF620 project part 1_calculus YLF.pdf`

