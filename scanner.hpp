#include <algorithm>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

enum tokenTypes
{
  boolToken,
  digitToken,
  keywordToken,
  operatorToken,
  stringToken
};

std::vector<std::pair<tokenTypes, std::string>> scan(std::string &input)
{
}

class scanner
{
private:
  template <class haystack>
  bool isInArray(haystack &haystack, std::string needle)
  {
    return std::find(std::begin(haystack), std::end(haystack), needle) !=
           std::end(haystack);
  }

  bool isDigit(const std::string &input)
  {
    return std::all_of(input.begin(), input.end(), ::isdigit);
  }

  bool isString(const std::string &input)
  {
    return input[0] == '""' && input[input.size() - 1] == '""';
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
  tokenTypes tokenType(std::string &token)
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
  }

public:
  tokenTypes tokenType(std::string &token)
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
  }

  template <typename C>
void split(std::string const &s, char const *d, C &ret)
{
  C output;

  std::bitset<255> delims;
  while (*d)
  {
    unsigned char code = *d++;
    delims[code] = true;
  }
  typedef std::string::const_iterator iter;
  iter beg;
  bool in_token = false;
  for (std::string::const_iterator it = s.begin(), end = s.end();
       it != end; ++it)
  {
    if (delims[*it])
    {
      if (in_token)
      {
        output.push_back(typename C::value_type(beg, it));
        in_token = false;
      }
    }
    else if (!in_token)
    {
      beg = it;
      in_token = true;
    }
  }
  if (in_token)
    output.push_back(typename C::value_type(beg, s.end()));
  output.swap(ret);
}

public:
  void analyze(const std::string &input)
  {
    char const* delims = " \t,()";
    std::vector<std::string_view> rawTokens;
    split(input, delims, rawTokens);
  }
};
