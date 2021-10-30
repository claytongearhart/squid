#include "console.hpp"
#include "scanner.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdint.h>
// using json = nlohmann::json;

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
    squid::object::node tokenTree("root");
    std::vector<unsigned int> locationVec = {};

    for (int i = 0; i < tokenList.size(); i++)
    {
        std::cout << "l38\n";

        if (tokenList[i].value == "{" && tokenList[i].type == squid::delimiterToken)
        {
            std::cout << "l42\n";
            tokenTree.getNodeByLocationVector(locationVec).addNode(squid::object::node("open"));
            locationVec.push_back(0);
        }
        else if (tokenList[i].value == "}" && tokenList[i].type == squid::delimiterToken)
        {
            std::cout << "l47\n";
            locationVec.pop_back();
        }
        else
        {
            std::cout << "l52\n";
            tokenTree.getNodeByLocationVector(locationVec).addNode("", squid::object::node({squid::object::node("type", tokenList[i].type), squid::object::node("value", tokenList[i].value)}) );
        }
    }

    std::cout << tokenTree.getJSON();
}