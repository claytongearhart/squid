#include <fstream>
#include <iostream>
#include "scanner.hpp"
#include "tree.hpp"
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

    // Find main function
    unsigned int mainAt = NULL;
    for (int i = 0; i < tokenList.size(); i++)
    {
        if (tokenList[i].value == "int" && tokenList[i + 1].value == "main")
        {
            mainAt = i + 1;
        } 
    }
    if (mainAt == NULL)
    {
        appConsole.error("No entry point.");
    }
}