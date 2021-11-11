#include "errors.hpp"
#include "utils.hpp"
#include <algorithm>
#include <string>
#include <utility>
#include <any>
#include <vector>

namespace squid
{
    class scope
    {

    };

namespace word
{
class any
{
};

class type
{

};

class variable
{
    public:
        std::any value;
    private:
        squid::scope accessScope;
        squid::word::type varType;
};
class function
{
  public:
    variable call(std::vector<variable> callArguments)
    {

    }
  private:
    std::vector<any> definition;
    std::vector<variable> arguments;
    squid::word::type returnType;

};
} // namespace word
} // namespace squid