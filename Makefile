build:
	mkdir -p bin
	g++ src/main.cpp -o bin/main -std=c++20

test:
	mkdir -p tests
	g++ tests/test.cpp -o tests/test -std=c++20 -pthread

