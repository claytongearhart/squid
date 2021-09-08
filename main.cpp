#include <fstream>
#include <iostream>

#include "scanner.hpp"

int main()
{
    scanner mainScanner;
    std::ifstream source("test_.cpp");
    std::string sourceString((std::istreambuf_iterator<char>(source)),
                 std::istreambuf_iterator<char>());

    mainScanner.analyze(sourceString);
    auto tokenList = mainScanner.fullTokens;
    for (int i = 0; i < tokenList.size(); i++)
    {
        std::cout << tokenList[i].second << '\n';
    }
}