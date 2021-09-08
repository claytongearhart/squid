#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <utility>
#include <vector>

bool isSpace(unsigned char c)
{
  return (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' ||
          c == '\f');
}
template <class object> bool isInArray(const object a[], object o)
{
  int length = sizeof(a) / sizeof(a[0]);
  for (int i = 0; i < length; i++)
  {
    if (o == a[i])
    {
      return true;
    }
  }

  return false;
}
enum tokenTypes
{
  boolToken,
  digitToken,
  keywordToken,
  operatorToken,
  stringToken,
  other
};

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
  tokenTypes tokenType(std::string token)
  {
    const std::string boolTokens[] = {"true", "false"},

                      delimiterTokens[] = {"(", ")", "{", "}", ";", ","},

                      keywordTokens[] = {"int",    "float", "auto",
                                         "double", "do",    "switch",
                                         "return"},

                      operatorTokens[] = {
                          "<", ">",  "<=", ">=", "*",  "+",  "-",  "/",
                          "=", "-=", "*=", "+=", "/=", "++", "--", "=="};

    if (isInArray(boolTokens, token))
    {
      return boolToken;
    }
    else if (isInArray(keywordTokens, token))
    {
      return keywordToken;
    }
    else if (isInArray(operatorTokens, token))
    {
      return operatorToken;
    }
    else if (isString(token))
    {
      return stringToken;
    }
    else if (isDigit(token))
    {
      return digitToken;
    }
    return other;
  }

public:
  std::vector<std::pair<tokenTypes, std::string>> fullTokens;

  void split(std::string const &s, std::vector<std::string> &values)
  {
    const char delims[] = {'\n', ' ', '(', ')'};
    std::vector<int> delimLocations;

    for (int i = 0; i < s.size(); i++)
    {
      if (isInArray(delims, s[i]))
      {
        delimLocations.push_back(i);
      }
    }
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
                                          tokenValues[i].end(), isSpace),
                           tokenValues[i].end());
      if (tokenValues[i] == "")
      {
        tokenValues.erase(tokenValues.begin() + i);
      }
    }
  }
};
