#include <algorithm>
#include <string>
#include <tuple>
#include <type_traits>
#include <variant>
#include <string.h>
#include <vector>

namespace squid
{

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

bool isInStringArray(const std::string a[], std::string o)
{
  for (int i = 0; i < a->size(); i++)
  {
    if (o == a[i])
    {
      return true;
    }
  }

  return false;
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