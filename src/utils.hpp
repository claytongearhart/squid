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

// template<class T>
//  class tree
//  {
//      public:
//          std::variant<T, tree> operator [](const std::string index)
//          {

//         }
//     private:
//         std::string json;
// };

namespace xml // Could use struct inside struct similar to how `nodeBase` can have it inside itself
{

class node
{
  public:
    std::vector<std::variant<node, std::string>> children;
    node(std::string name, std::map<std::string, std::string> tagProps,
         std::variant<node, std::string> content)
    {
        tagName = name;
        props = tagProps;
        addChild(0, content);
    }
    node operator[](int index)
    {
        return std::get<node>(children[index]);
    }

    const node &operator[](int index) const
    {
        return std::get<node>(children[index]);
    }

    void operator = (const std::variant<node, std::string> &value)
    {
        children.empty();
        children[0] = value;
    }

    std::variant<node, std::string> addChild(unsigned int index,
                                             std::variant<node, std::string> child)
    {
        childrenIt = children.begin();
        std::advance(childrenIt, index);

        children.insert(childrenIt, child);

        return children[index];
    }

    std::string getRawXML()
    {
        std::string xmlString;
        xmlString = fmt::format("<{0} {1}>\n\t", tagName, getPropString(props));
        getChildContent(this, xmlString);
        xmlString += fmt::format("\n</{}>", tagName);
        return xmlString;
    }

  private:

    std::map<std::string, std::string> props;
    std::string tagName;
    std::vector<std::variant<node, std::string>>::iterator childrenIt;

    std::string getPropString(std::map<std::string, std::string> &inputProps)
    {
        std::string propsString;
        std::map<std::string, std::string>::iterator it;
        for (it = inputProps.begin(); it != inputProps.end(); ++it)
        {
            propsString += it->first + "=\"" + it->second + "\"";
        }

        return propsString;
    }

    void getChildContent(node *input, std::string &xmlString)
    {
        for (int i = 0; i < input->children.size(); i++)
        {
            if (input->children[i].index() == 1)
            {
                xmlString += std::get<std::string>(input->children[i]);
            }
            else // Type of input -> children[i] is assumed to be squid::xml::node here
            {
                xmlString += fmt::format("<{0} {1}>", std::get<node>(input->children[i]).tagName,
                                         getPropString(props));
                getChildContent(&std::get<node>(input->children[i]), xmlString);

                xmlString += fmt::format("</{0}>", std::get<node>(input->children[i]).tagName);
            }
        }
    }
};
class document
{
  public:
    document(std::string version)
    {
        rawTree = "<?xml version=\"" + version + "\" encoding=\"UTF-8\"?>\n";
    }

    void addNode(std::string tagName, std::map<std::string, std::string> props, std::string content)
    {
        std::string propsString;
        std::map<std::string, std::string>::iterator it;
        for (it = props.begin(); it != props.end(); ++it)
        {
            propsString += it->first + "=\"" + it->second + "\"";
        }
        std::string innerXML =
            fmt::format("<{0} {1}>\n\t{2}\n</{0}>", tagName, propsString, content);

        rawTree += innerXML;
    }

    std::string getRawData()
    {
        return rawTree;
    }

  private:
    std::string rawTree;
};

}; // namespace xml

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