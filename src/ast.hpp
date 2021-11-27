#include "errors.hpp"
#include "json.hpp"
#include "utils.hpp"
#include <algorithm>
#include <any>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace squid
{
namespace AST
{

class node
{
  public:
    node *leftNode;
    node *rightNode;

    node(std::any value)
    {

        leftNode = rightNode = NULL;
    }
};
class parser
{
  public:
    parser()
    {
    }

  protected:
    squid::AST::node binaryOp(std::vector<squid::token> input)
    {
        size_t opLoc;
        for (int i = 0; i < input.size(); i++)
        {
            if ( input[i].type == squid::tokenTypes::operatorToken)
            {

            }
        }
    }
};
} // namespace AST
}; // namespace squid