#include "json.hpp"
#include <algorithm>
#include <array>
#include <fmt/format.h>
#include <list>
#include <map>
#include <string.h>
#include <string>
#include <tuple>
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
    {
    }
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

namespace object
{
class object
{
  public:
    bool isLiteral;
};
class string : public object
{
  public:
    std::string value;
};
} // namespace object
class token
{
  public:
    tokenTypes type;
    size_t location;
    std::string value;
    std::string scopeDepth;

    token(tokenTypes inputType, std::string inputValue, size_t inputLocation)
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
    return (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == '\f');
}

bool isInStringVec(std::vector<std::string> a, std::string o)
{
    return std::find(a.begin(), a.end(), o) != a.end();
}
bool isInCharArray(std::string a, const char o)
{
    for (int i = 0; i < a.length(); i++)
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