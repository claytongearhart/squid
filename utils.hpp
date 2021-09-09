#include <type_traits>
#include <variant>
#include <vector>
#include <string>

namespace squid
{

enum tokenTypes
{
  boolToken,
  digitToken,
  keywordToken,
  operatorToken,
  stringToken,
  other
};


namespace utils
{
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
} // namespace utils

} // namespace squid