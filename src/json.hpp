#include <algorithm>
#include <array>
#include <fmt/format.h>
#include <list>
#include <map>
#include <span>
#include <string.h>
#include <string>
#include <tuple>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

namespace squid
{
namespace utils
{
std::string repeatChar(int times, char c)
{
    std::string returnValue;
    for (int i = 0; i < times; i++)
    {
        returnValue += c;
    }

    return returnValue;
}
} // namespace utils
namespace object
{
class node
{
  public:
    std::vector<std::variant<node, std::string>> children;
    node(std::string name, std::optional<std::variant<node, std::string>> content = {})
    {
        tagName = name;
        if (content.has_value())
        {
            addNode(content.value(), 0);
        }
        
    }
    node operator[](int index)
    {
        return std::get<node>(children[index]);
    }

    const node &operator[](int index) const
    {
        return std::get<node>(children[index]);
    }

    size_t size()
    {
        return children.size();
    }


    node accessNode(node n, std::span<unsigned int> span)
    {
        return span.size() ? accessNode(n[span[0]], span.subspan(1)) : n;
    }

    node getNodeByLocationVector(std::vector<unsigned int> locationVec)
    {
        return accessNode(*this, locationVec);
    }

    std::variant<node, std::string> addNode(std::variant<node, std::string> child, std::optional< unsigned int> index = {})
    {
        if (index.has_value())
        {
        childrenIt = children.begin();
        std::advance(childrenIt, index.value());

        children.insert(childrenIt, child);

        return children[index.value()];
        }
        else {
            children.insert(children.end(), child);
            return children[children.size()];
        }
    }
    std::string tagName;

  private:
    std::vector<std::variant<node, std::string>>::iterator childrenIt;
};
}; // namespace xml

} // namespace squid
