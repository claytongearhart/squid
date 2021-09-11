#include <algorithm>
#include <string.h>
#include <string>
#include <tuple>
#include <array>
#include <type_traits>
#include <variant>
#include <vector>

namespace squid
{

struct bracket
{
  int position;
  bool isOpener;

  bracket(int p, bool iO) : position{p}, isOpener{iO}
  {} 
};

enum tokenTypes
{
  boolToken,
  delimiterToken,
  digitToken,
  keywordToken,
  operatorToken,
  stringToken,
  other
};

class token
{
public:
  tokenTypes type;
  int location;
  std::string value;

  token(tokenTypes inputType, std::string inputValue, int inputLocation)
  {
    type = inputType;
    value = inputValue;
    location = inputLocation;
  }
};

namespace utils
{
bool isSpace(unsigned char c)
{
  return (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' ||
          c == '\f');
}

bool isInStringVec(std::vector<std::string> a, std::string o)
{
  return std::find(a.begin(), a.end(), o) != a.end();

}
bool isInCharArray(const char a[], const char o)
{
  for (int i = 0; i < strlen(a); i++)
  {
    if (o == a[i])
    {
      return true;
    }
  }

  return false;
}
} // namespace utils

} // namespace squid