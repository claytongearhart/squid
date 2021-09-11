#include "../lib/tree.hpp"
#include "console.hpp"
#include "scanner.hpp"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <stdint.h>


int main()
{
  scanner mainScanner;
  squid::console appConsole;

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

  tree<squid::token> scopeTree;
  std::vector<tree<squid::token>::iterator> scopeTreeNodes = {
      scopeTree.begin(), scopeTree.insert(scopeTreeNodes[0], {squid::token(squid::other, "ROOT", 0)})};
  uint8_t scopeDepth = 0;
  for (int i = 1; i < bracketLocations.size(); i++)
  {
    if (bracketLocations[i].isOpener)
    {
      scopeDepth++;
    }
    else
    {
      scopeDepth--;
    }

    for (int j = 0; j < bracketLocations[i + 1].position -
                            bracketLocations[i].position;
         j++)
    {
      scopeTreeNodes[i] = scopeTree.append_child(
          scopeTreeNodes[scopeDepth],
          tokenList[bracketLocations[i].position + j]);
    }
  }
}
// tokenList[bracketLocations[i].position]