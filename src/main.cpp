//#include "../lib/tree.hpp"
#include "console.hpp"
#include "scanner.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdint.h>
int main()
{
    scanner mainScanner;
    squid::console appConsole;

    std::cout << "l13\n"; // pass

    std::ifstream source("test_.spp");
    std::string sourceString((std::istreambuf_iterator<char>(source)),
                             std::istreambuf_iterator<char>());

    mainScanner.analyze(sourceString);
    std::vector<squid::token> tokenList = mainScanner.fullTokens;

    std::cout << mainScanner.fullTokens.size();
    for (int i = 0; i < tokenList.size(); i++)
    {
        std::cout << tokenList[i].value << " : " << tokenList[i].type
                  << '\n';
    }

    unsigned int mainAt;
    bool mainExist;

    std::vector<squid::bracket> bracketLocations;

    for (int i = 0; i < tokenList.size(); i++)
    {
        if (tokenList[i].value == "int" &&
            tokenList[i].type == squid::keywordToken &&
            tokenList[i + 1].value == "main")
        {
            mainAt = i + 1;
            bool mainExist = true;
        }
        // Search for braces
        if (tokenList[i].value == "{" &&
            tokenList[i].type == squid::delimiterToken)
        {
            bracketLocations.emplace_back(squid::bracket(i, true));
        }
        if (tokenList[i].value == "}" &&
            tokenList[i].type == squid::delimiterToken)
        {
            bracketLocations.emplace_back(squid::bracket(i, false));
        }
    }
}
