build:
	mkdir -p bin
	g++ src/main.cpp -o squid/squid -std=c++20
debug:
	mkdir -p bin
	g++ src/main.cpp -o squid/squid -std=c++20 -ggdb	
