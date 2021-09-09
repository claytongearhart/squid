#include <fstream>
#include <iostream>
#include "scanner.hpp"
#include "tree.hpp"

int main()
{
    scanner mainScanner;
    std::ifstream source("test_.cpp");
    std::string sourceString((std::istreambuf_iterator<char>(source)),
                 std::istreambuf_iterator<char>());

    tree <std::pair<squid::tokenTypes, std::string>> tr;

    mainScanner.analyze(sourceString);
    auto tokenList = mainScanner.fullTokens;
    for (int i = 0; i < tokenList.size(); i++)
    {
        std::cout << tokenList[i].second << '\n';
    }
}