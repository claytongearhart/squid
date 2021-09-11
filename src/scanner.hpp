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
  std::vector<std::string> boolTokens = {"true", "false"},

                    delimiterTokens = {"(", ")", "{", "}", ";", ","},

                    keywordTokens = {"int", "float",  "auto",  "double",
                                        "do",  "switch", "return"},

                    operatorTokens = {
                        "<", ">",  "<=", ">=", "*",  "+",  "-",  "/",
                        "=", "-=", "*=", "+=", "/=", "++", "--", "=="};
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

    return squid::utils::isInStringVec(operatorTokens, token)
               ? squid::boolToken
           : squid::utils::isInStringVec(delimiterTokens, token)
               ? squid::delimiterToken
           : squid::utils::isInStringVec(keywordTokens, token)
               ? squid::keywordToken
           : squid::utils::isInStringVec(boolTokens, token)
               ? squid::operatorToken
           : isString(token) ? squid::stringToken
           : isDigit(token)  ? squid::digitToken
                             : squid::other;
  }

public:
  std::vector<squid::token> fullTokens;

  void split(std::string const &s, std::vector<std::string> &values)
  {
    const char delims[] = {'\n', ' ', '(', ')', ';'};
    std::vector<int> delimLocations;

    for (int i = 0; i < s.size(); i++)
    {
      if (squid::utils::isInCharArray(delims, s[i]))
      {
        delimLocations.push_back(i);
      }
    }
    values.emplace_back(s.substr(0, delimLocations[0]));
    for (int i = 0; i < delimLocations.size(); i++)
    {
      values.emplace_back(s.substr(delimLocations[i], delimLocations[i + 1] -
                                                       delimLocations[i]));
    }
  }
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
      fullTokens.push_back({tokenType(tokenValues[i]), tokenValues[i], 1});
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
