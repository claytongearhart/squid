

build:
	mkdir -p bin
	g++ src/scanner.cpp -o bin/scanner -std=c++20 -lfmt

debug:
	mkdir -p bin
	g++ src/scanner.cpp -o bin/scanner -std=c++20 -lfmt -ggdb

