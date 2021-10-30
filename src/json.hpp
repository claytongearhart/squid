#include <algorithm>
#include <any>
#include <array>
#include <concepts>
#include <cstddef>
#include <fmt/format.h>
#include <functional>
#include <list>
#include <map>
#include <optional>
#include <span>
#include <string.h>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <variant>
#include <vector>

template <typename T>
concept Number = std::is_integral_v<T> || std::is_floating_point_v<T>;

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

    class child
    {
      public:
        child(const char input[])
        {
            value = input;
            type = string;
        }

        child(squid::tokenTypes input)
        {
            value = input;
            type = tokenTypesEnum;
        }

        child(int input)
        {
            value = input;
            type = num;
        }

        operator std::string()
        {
            std::string returnValue;
            switch (type)
            {
            case string:
                returnValue = fromString(std::get<std::string>(value));
            case num:
                returnValue = fromNum(std::get<int>(value));
            case tokenTypesEnum:
                returnValue = fromTokenTypesEnum(std::get<tokenTypes>(value));
            }
        }

      private:
        std::variant<std::string, int, squid::tokenTypes> value;
        enum types
        {
            string,
            tokenTypesEnum,
            num
        };
        types type;

        // Converters
        inline std::string fromString(std::string &input)
        {
            return "\"" + input + "\"";
        }

        template <typename num>
        requires Number<num>
        inline std::string fromNum(num input)
        {
            return std::to_string(input);
        }

        std::string fromTokenTypesEnum(squid::tokenTypes input)
        {
            std::string returnValue;
            switch (input)
            {
            case boolToken:
                returnValue = "Boolean Token";
            case delimiterToken:
                returnValue = "Delimiter Token";
            case digitToken:
                returnValue = "Digit Token";
            case keywordToken:
                returnValue = "Keyword Token";
            case operatorToken:
                returnValue = "Operator Token";
            case stringToken:
                returnValue = "String Token";
            default:
                "Other token";
            };
        }
    };

    class node
    {
      public:
        std::vector<std::variant<node, child>> children;
        node(std::string name,
             std::optional<std::variant<std::variant<node, child>, std::vector<node>>>
                 content = {})
        {
            tagName = name;
            if (content.has_value())
            {
                if (content.value().index() != 2)
                {
                    addNode(std::get<std::variant<node, child>>(content.value()), 0);
                }

                else
                {
                    auto &tempContent = std::get<std::vector<node>>(content.value());

                    for (int i = 0; i < tempContent.size(); i++)
                    {
                        addNode(tempContent[i]);
                    }
                }
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

        node &accessNode(node *n, std::span<unsigned int> span)
        {
            return span.size() ? accessNode(&n[span[0]], span.subspan(1)) : *n;
        }

        node &getNodeByLocationVector(std::vector<unsigned int> locationVec)
        {
            return accessNode(this, locationVec);
        }

        void addNode(std::variant<node, child> child, std::optional<unsigned int> index = {})
        {
            if (index.has_value())
            {
                childrenIt = children.begin();
                std::advance(childrenIt, index.value());

                children.insert(childrenIt, child);

                return;
            }
            else
            {
                children.insert(children.end(), child);
                return;
            }
        }
        std::string tagName;

        std::string getJSON()
        {
            std::string xmlString = fmt::format({"\"{}\":\n{{\n"}, tagName);
            for (int i = 0; i < children.size(); i++)
            {
                if (children[i].index() == 1)
                {
                    xmlString += std::get<std::string>(children[i]);
                }
                else // Type of input -> children[i] is assumed to be squid::xml::node here
                {
                    xmlString += fmt::format("{}\"{}\": ", squid::utils::repeatChar(2, ' '),
                                             std::get<node>(children[i]).tagName);
                    getChildContent(&std::get<node>(children[i]), xmlString, 2);

                    xmlString += i + 1 < children.size() ? "\n" : "";
                }
            }
            xmlString += "\n}";

            return xmlString;
        }

      private:
        void getChildContent(node *input, std::string &xmlString, unsigned int indentDepth)
        {
            for (int i = 0; i < input->children.size(); i++)
            {
                if (input->children[i].index() == 1)
                {
                    xmlString += std::get<std::string>(input->children[i]);
                }
                else // Type of input -> children[i] is assumed to be squid::xml::node here
                {
                    indentDepth += 2;
                    xmlString += fmt::format(
                        "{}\"{}\":{}", squid::utils::repeatChar(indentDepth, ' '), input->tagName,
                        input->children[i].index() == 1
                            ? " "
                            : "\n" + squid::utils::repeatChar(indentDepth, ' ') + "{\n");
                    getChildContent(&std::get<node>(input->children[i]), xmlString,
                                    indentDepth + 2);

                    xmlString += i + 1 < children.size() ? "\n" : "";
                }
            }
        }
        std::vector<std::variant<node, child>>::iterator childrenIt;
    };
    }; // namespace object

} // namespace squid
