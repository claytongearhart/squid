#include <fstream>
#include <iostream>
#include "scanner.hpp"
#include "../lib/tree.hpp"
#include "console.hpp"

int main()
{
    scanner mainScanner;
    squid::console appConsole;
    tree <std::pair<squid::tokenTypes, std::string>> tr;

    std::ifstream source("test_.cpp");
    std::string sourceString((std::istreambuf_iterator<char>(source)),
                 std::istreambuf_iterator<char>());

    mainScanner.analyze(sourceString);
    auto tokenList = mainScanner.fullTokens;
    for (int i = 0; i < tokenList.size(); i++)
    {
        std::cout << tokenList[i].value << " : " << tokenList[i].type << '\n';
    }

    unsigned int mainAt;
    bool mainExist;

    std::vector <squid::bracket> bracketLocations;

    for (int i = 0; i < tokenList.size(); i++)
    {
        if (tokenList[i].value == "int" && tokenList[i].type == squid::keywordToken && tokenList[i + 1].value == "main")
        {
            mainAt = i + 1;
            bool mainExist = true;
        }
        // Search for braces
        if (tokenList[i].value == "{" && tokenList[i].type == squid::delimiterToken)
        {
            bracketLocations.emplace_back(squid::bracket(i, true));
        }
        if (tokenList[i].value == "{" && tokenList[i].type == squid::delimiterToken)
        {
            bracketLocations.emplace_back(squid::bracket(i, false));
        }
    }
    if (mainExist)
    {
        appConsole.error("No entry point.");
    }


    

}
