oop1: 	main.o Market.o Date.o
	g++  -o oop.out main.o Market.o Date.o
# market.Print() member function illustration
marketPrint : Market.o Date.o marketPrint.cpp
	g++ -c marketPrint.cpp
	g++ -o marketPrint.out Market.o Date.o marketPrint.o
# copy constructor of class Market
copyCtor : Market.o Date.o copyCtor.cpp
	g++ -c copyCtor.cpp
	g++ -o copyCtor.out Market.o Date.o copyCtor.o
# example of having pointer in class Market
copyCtor2 : MarketTemp.o Date.o copyCtor2.cpp
	g++ -c copyCtor2.cpp
	g++ -o copyCtor2.out MarketTemp.o Date.o copyCtor2.o
# the mainline of our example
main.o: main.cpp TreePricer.h TreeProduct.h
	g++ -c main.cpp
Market.o: Market.cpp Market.h Date.h
	g++ -c Market.cpp 
Date.o: Date.cpp Date.h
	g++ -c Date.cpp

oop2: 	main.o Market.o Date.o Barrier.o
	g++  -o oop.out main.o Market.o Date.o Barrier.o
# the mainline of our example
main.o: main.cpp TreePricer.h TreeProduct.h EuropeanTrade.h AmericanTrade.h BarrierTrade.h Barrier.h
	g++ -c main.cpp
Market.o: Market.cpp Market.h Date.h
	g++ -c Market.cpp
Date.o: Date.cpp Date.h
	g++ -c Date.cpp
Barrier.o : Barrier.h Barrier.cpp
	g++ -c Barrier.cpp

# clean all output files
clean:
	rm *.o *.out
