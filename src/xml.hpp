#include <algorithm>
#include <array>
#include <fmt/format.h>
#include <list>
#include <map>
#include <span>
#include <string.h>
#include <string>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

namespace squid
{
namespace xml
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
        addNode(0, content);
    }
    node operator[](int index)
    {
        return std::get<node>(children[index]);
    }

    const node &operator[](int index) const
    {
        return std::get<node>(children[index]);
    }

    void operator=(const std::variant<node, std::string> &value)
    {
        children.clear();
        children.push_back(value);
    }

    void operator+=(const std::variant<node, std::string> &value)
    {
        children.push_back(value);
    }

    std::string getXML()
    {
        std::string xmlString;
        for (int i = 0; i < children.size(); i++)
        {
            if (children[i].index() == 1)
            {
                xmlString += std::get<std::string>(children[i]);
            }
            else // Type of input -> children[i] is assumed to be squid::xml::node here
            {
                xmlString += fmt::format("<{0} {1}>", std::get<node>(children[i]).tagName,
                                         getPropString(std::get<node>(children[i]).props));
                getChildContent(&std::get<node>(children[i]), xmlString);

                xmlString += fmt::format("</{0}>", std::get<node>(children[i]).tagName);
            }
        }

        return xmlString;
    }


    node accessNode(node n, std::span<unsigned int> span)
    {
        return span.size() ? accessNode(n[span[0]], span.subspan(1)) : n;
    }

    node getNodeByLocationVector(std::vector<unsigned int> locationVec)
    {
        return accessNode(std::get<node>(children[1]), locationVec);
    }

    std::variant<node, std::string> addNode(unsigned int index,
                                            std::variant<node, std::string> child)
    {
        childrenIt = children.begin();
        std::advance(childrenIt, index);

        children.insert(childrenIt, child);

        return children[index];
    }
    std::map<std::string, std::string> props;
    std::string tagName;

  private:
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
}; // namespace xml
// class document
// {
//   public:
//     document(std::string version)
//     {
//         children.push_back("<?xml version=\"" + version + "\" encoding=\"UTF-8\"?>\n");
//     }

//     squid::xml::node operator[](const unsigned int index)
//     {
//         return std::get<squid::xml::node>(children[index + 1]);
//     }

//     const squid::xml::node &operator[](const unsigned int index) const
//     {
//         return std::get<squid::xml::node>(children[index + 1]);
//     }

//     node accessNode(node n, std::span<unsigned int> span)
//     {
//         return span.size() ? accessNode(n[span[0]], span.subspan(1)) : n;
//     }

//     node getNodeByLocationVector(std::vector<unsigned int> locationVec)
//     {
//         return accessNode(std::get<node>(children[1]), locationVec);
//     }

//     std::variant<node, std::string> addNode(unsigned int index,
//                                             std::variant<node, std::string> child)
//     {
//         childrenIt = children.begin();
//         std::advance(childrenIt, index + 1);

//         children.insert(childrenIt, child);

//         return children[index + 1];
//     }

//     std::string getXML()
//     {
//         std::string xmlString;
//         for (int i = 0; i < children.size(); i++)
//         {
//             if (children[i].index() == 1)
//             {
//                 xmlString += std::get<std::string>(children[i]);
//             }
//             else // Type of input -> children[i] is assumed to be squid::xml::node here
//             {
//                 xmlString += fmt::format("<{0} {1}>", std::get<node>(children[i]).tagName,
//                                          getPropString(std::get<node>(children[i]).props));
//                 getChildContent(&std::get<node>(children[i]), xmlString);

//                 xmlString += fmt::format("</{0}>", std::get<node>(children[i]).tagName);
//             }
//         }

//         return xmlString;
//     }

//   private:
//     std::vector<std::variant<node, std::string>> children;
//     std::vector<std::variant<node, std::string>>::iterator childrenIt;

//     std::string getPropString(std::map<std::string, std::string> &inputProps)
//     {
//         std::string propsString;
//         std::map<std::string, std::string>::iterator it;
//         for (it = inputProps.begin(); it != inputProps.end(); ++it)
//         {
//             propsString += it->first + "=\"" + it->second + "\"";
//         }

//         return propsString;
//     }

//     void getChildContent(node *input, std::string &xmlString)
//     {
//         for (int i = 0; i < input->children.size(); i++)
//         {
//             if (input->children[i].index() == 1)
//             {
//                 xmlString += std::get<std::string>(input->children[i]);
//             }
//             else // Type of input -> children[i] is assumed to be squid::xml::node here
//             {
//                 xmlString += fmt::format("<{0} {1}>", std::get<node>(input->children[i]).tagName,
//                                          getPropString(std::get<node>(input->children[i]).props));
//                 getChildContent(&std::get<node>(input->children[i]), xmlString);

//                 xmlString += fmt::format("</{0}>", std::get<node>(input->children[i]).tagName);
//             }
//         }
//     }
//};

}; // namespace xml

} // namespace squid
