#include "utils.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <utility>
#include <vector>

class scanner
{
private:
  std::vector<std::string> tokenValues;
  bool isDigit(const std::string &input)
  {
    return std::all_of(input.begin(), input.end(), ::isdigit);
  }

  bool isString(const std::string &input)
  {
    return input[0] == '\"' && input[input.size() - 1] == '\"';
  }

  void sanatize(std::string &input)
  {
    int i;
    for (char const &character : input)
    {
      if (character == '\n')
      {
        input.erase(input.begin() + i);
      }
      i++;
    }
  }
  squid::tokenTypes tokenType(std::string token)
  {
    const std::string boolTokens[] = {"true", "false"},

                      delimiterTokens[] = {"(", ")", "{", "}", ";", ","},

                      keywordTokens[] = {"int",    "float", "auto",
                                         "double", "do",    "switch",
                                         "return"},

                      operatorTokens[] = {
                          "<", ">",  "<=", ">=", "*",  "+",  "-",  "/",
                          "=", "-=", "*=", "+=", "/=", "++", "--", "=="};

    if (squid::utils::isInArray(boolTokens, token))
    {
      return squid::boolToken;
    }
    else if (squid::utils::isInArray(keywordTokens, token))
    {
      return squid::keywordToken;
    }
    else if (squid::utils::isInArray(operatorTokens, token))
    {
      return squid::operatorToken;
    }
    else if (isString(token))
    {
      return squid::stringToken;
    }
    else if (isDigit(token))
    {
      return squid::digitToken;
    }
    return squid::other;
  }

public:
  std::vector<std::pair<squid::tokenTypes, std::string>> fullTokens;

  void split(std::string const &s, std::vector<std::string> &values)
  {
    const char delims[] = {'\n', ' ', '(', ')', ';'};
    std::vector<int> delimLocations;

    for (int i = 0; i < s.size(); i++)
    {
      if (squid::utils::isInArray(delims, s[i]))
      {
        delimLocations.push_back(i);
      }
    }
    values.push_back(s.substr(0, delimLocations[0]));
    for (int i = 0; i < delimLocations.size(); i++)
    {
      values.push_back(s.substr(delimLocations[i], delimLocations[i + 1] -
                                                       delimLocations[i]));
    }
  }

public:
  void analyze(const std::string input)
  {
    std::vector<std::string> rawTokens;
    split(input, rawTokens);
    tokenValues = rawTokens;
    sanatizeTokens();
    calcTypes();
  }

  void calcTypes()
  {
    for (int i = 0; i < tokenValues.size(); i++)
    {
      fullTokens.push_back(
          std::make_pair(tokenType(tokenValues[i]), tokenValues[i]));
    }
  }

  void sanatizeTokens()
  {
    for (int i = 0; i < tokenValues.size(); i++)
    {
      tokenValues[i].erase(std::remove_if(tokenValues[i].begin(),
                                          tokenValues[i].end(),
                                          squid::utils::isSpace),
                           tokenValues[i].end());
      if (tokenValues[i] == "")
      {
        tokenValues.erase(tokenValues.begin() + i);
      }
    }
  }
};
