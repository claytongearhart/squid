#include "console.hpp"
#include "scanner.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdint.h>
//using json = nlohmann::json;

int main()
{

    scanner mainScanner;
    squid::console appConsole;

    std::ifstream source("test_.spp");
    std::string sourceString((std::istreambuf_iterator<char>(source)),
                             std::istreambuf_iterator<char>());

    mainScanner.analyze(sourceString);
    std::vector<squid::token> tokenList = mainScanner.fullTokens;

    std::cout << mainScanner.fullTokens.size();
    for (int i = 0; i < tokenList.size(); i++)
    {
        std::cout << tokenList[i].value << " : " << tokenList[i].type << '\n';
    }

    unsigned int mainAt;
    bool mainExist;

    std::vector<squid::bracket> bracketLocations;

    for (int i = 0; i < tokenList.size(); i++)
    {
        if (tokenList[i].value == "int" && tokenList[i].type == squid::keywordToken &&
            tokenList[i + 1].value == "main")
        {
            mainAt = i + 1;
            bool mainExist = true;
        }
        // Search for braces
        if (tokenList[i].value == "{" && tokenList[i].type == squid::delimiterToken)
        {
            bracketLocations.emplace_back(squid::bracket(i, true));
        }
        if (tokenList[i].value == "}" && tokenList[i].type == squid::delimiterToken)
        {
            bracketLocations.emplace_back(squid::bracket(i, false));
        }
    }

    squid::xml::node xmlNode("p", {{"boo", "far"}}, squid::xml::node("p", {{"boo", "far"}}, ""));



    std::cout << xmlNode.getNodeByLocationVector({0}).getXML() << "\n";

}