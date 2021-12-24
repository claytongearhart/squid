#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <stdint.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace squid
{

const std::set<std::string> boolTokens{"true", "false"};
const std::set<std::string> delimiterTokens{"(", ")", "{", "}", ";", ","};
const std::set<std::string> keywordTokens{"int", "auto", "do", "switch", "return", "class"};
const std::set<std::string> typeTokens{"int", "float", "double"};
const std::set<std::string> operatorTokens{"<",  ">",  "<=", ">=", "*",  "+",  "-",  "/",  "=",
                                           "-=", "*=", "+=", "/=", "++", "--", "==", "%=", "!=",
                                           "!",  "||", "&&", "&",  "|",  "~",  "^"};
enum tokenTypes
{
    boolToken,
    delimiterToken,
    digitToken,
    keywordToken,
    operatorToken,
    stringToken,
    typeToken,
    varNameToken,
    other
};

std::string tokenTypeToString(tokenTypes input)
{
    switch (input)
    {
    case boolToken:
        return "Boolean Token";
    case delimiterToken:
        return "Delimiter Token";
    case digitToken:
        return "Digit Token";
    case keywordToken:
        return "Keyword Token";
    case operatorToken:
        return "Operator Token";
    case stringToken:
        return "String Token";
    case typeToken:
        return "Type Token";
    case varNameToken:
        return "Variable Name Token";
    case other:
        return "Unknown Token Type";
    };
}

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

inline bool isInSS(std::set<std::string> a, std::string o)
{
    return a.count(o);
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

class scanner
{
  private:
    std::vector<std::pair<int, int>> stringLocations;
    std::vector<std::pair<std::string, int>> tokenValues;

    void findStrings(std::string input)
    {
        std::vector<int> quoteLocations;
        std::vector<int> dQuoteLocations;

        for (int i = 0; i < input.size(); i++)
        {
            bool isEscaped = input[i - 1] == '\\';

            if (input[i] == '\"' && !isEscaped)
            {
                dQuoteLocations.push_back(i);
            }
            else if (input[i] == '\'' && !isEscaped)
            {
                quoteLocations.push_back(i);
            }
        }
        for (int i = 0; i < quoteLocations.size(); i += 2)
        {
            stringLocations.emplace_back(std::make_pair(quoteLocations[i], quoteLocations[i + 1]));
        }
        for (int i = 0; i < dQuoteLocations.size(); i += 2)
        {
            stringLocations.emplace_back(
                std::make_pair(dQuoteLocations[i], dQuoteLocations[i + 1]));
        }

        cleanStringLocations();
    }

    void cleanStringLocations()
    {
        size_t stringStart, stringEnd;
        for (int i = 0; i < stringLocations.size(); i++)
        {
            stringStart = stringLocations[i].first;
            stringEnd = stringLocations[i].second;

            for (int j = 0; j < stringLocations.size(); j++)
            {
                if (stringLocations[i].first < stringLocations[j].first &&
                    stringLocations[j].first < stringLocations[i].second)
                {
                    stringLocations.erase(stringLocations.begin() + j);
                }
            }
        }
    }
    bool isInString(int location)
    {

        for (int i = 0; i < stringLocations.size(); i++)
        {
            if ((stringLocations[i].first + 1 < location) &&
                (location < stringLocations[i].second - 1))
            {
                return true;
            }
        }
        return false;
    }

    size_t find_first_of_delim(std::string input, std::string delims, size_t lastPos)
    {

        for (int i = lastPos; i < input.size(); i++)
        {
            if ((delims.find(input[i]) != std::string::npos) && !isInString(i))
            {
                return i;
            }
        }

        return std::string::npos;
    }
    squid::tokenTypes tokenType(std::string token)
    {

        return squid::utils::isInSS(operatorTokens, token)    ? squid::operatorToken
               : squid::utils::isInSS(delimiterTokens, token) ? squid::delimiterToken
               : squid::utils::isInSS(keywordTokens, token)   ? squid::keywordToken
               : squid::utils::isInSS(boolTokens, token)      ? squid::boolToken
               : isString(token)                              ? squid::stringToken
               : isDigit(token)                               ? squid::digitToken
                                                              : squid::other;
    }

    // Tokens would have remnant of last token on current token if size < 1 so this removes it
    void refineTokens()
    {
        std::vector<squid::token> tempFullTokens;
        for (int i = 0; i < fullTokens.size(); i++)
        {
            if (fullTokens[i].value == " ")
            {
                // Skips token therefore deleting it
            }

            else if (fullTokens[i].value.length() != 1)
            {
                auto beginTokenValue =
                    fullTokens[i].value.substr(0, fullTokens[i].value.size() - 1);
                std::string lastTokenValue(1, fullTokens[i].value.back());

                tempFullTokens.push_back(
                    {tokenType(beginTokenValue), beginTokenValue, fullTokens[i].location});
                tempFullTokens.push_back(
                    {tokenType(lastTokenValue), lastTokenValue, fullTokens[i].location});
            }
            else
            {
                tempFullTokens.push_back(fullTokens[i]);
            }
        }
        fullTokens = tempFullTokens;
        tempFullTokens.clear();

        std::string operatorDelims = "<>:=+-";
        for (int i = 0; i < fullTokens.size(); i++)
        {
            if (fullTokens[i].value.length() == 1 &&
                squid::utils::isInCharArray(operatorDelims, fullTokens[i].value.front()) &&
                fullTokens[i].value == fullTokens[i + 1].value)
            {
                std::string token;

                token = fullTokens[i].value;
                token += fullTokens[i + 1].value;

                tempFullTokens.push_back({squid::operatorToken, token, fullTokens[i].location});

                i++;
            }
            else if (fullTokens[i].type == squid::stringToken &&
                     fullTokens[i + 2].type == squid::stringToken)
            {
                tempFullTokens.push_back(
                    {squid::stringToken, fullTokens[i + 1].value, fullTokens[i + 1].location});
                i += 2;
            }
            else
            {
                tempFullTokens.push_back(fullTokens[i]);
            }
        }

        fullTokens = tempFullTokens;
    }

  public:
    std::vector<squid::token> fullTokens;

    static bool isDigit(const std::string &input)
    {
        return std::all_of(input.begin(), input.end(), ::isdigit);
    }

    static bool isString(const std::string &input)
    {
        return input[0] == '\"' && input[input.size() - 1] == '\"';
    }

    void split(std::string s)
    {
        size_t pos = 0, lastPos = 0;

        std::cout << s;

        while ((pos = find_first_of_delim(s, " []{}()<>+-*/&:;.\n\"", lastPos)) !=
               std::string::npos)
        {
            std::string token = s.substr(lastPos, pos - lastPos + 1);

            fullTokens.push_back({tokenType(token), token, pos});

            lastPos = pos + 1;
        }
    }
    void sanitizeTokens()
    {
        for (int i = 0; i < fullTokens.size(); i++)
        {
            if (squid::utils::isSpace(fullTokens[i].value[0]))
            {
                fullTokens.erase(fullTokens.begin() + i);
            }
        }
    }
    void analyze(std::string input)
    {
        findStrings(input);
        split(input);
        refineTokens();
        sanitizeTokens();
    }
};

class object
{
  public:
    object(std::string t)
    {
        type = t;
    }

  private:
    std::string type;
};

class stage2_anal
{
  public:
    std::vector<squid::token> analSex(std::vector<token> input)
    {
        std::vector<squid::token> tempTokens;

        for (int i = 0; i < input.size(); i++)
        {
            if (input[i].value == "class")
            {
                typenameRef.insert(input[i + 1].value);
            }

            squid::tokenTypes tokenType;
            if (typenameRef.count(input[i].value))
            {
                tokenType = squid::typeToken;
            }
            else if (((tempTokens[i - 1].type == squid::typeToken) &&
                      (input[i - 2].value != "class")))
            {
                objectRef.insert({input[i].value, squid::object(input[i - 1].value)});
                tokenType = squid::varNameToken;
            }
            else if (objectRef.count(input[i].value))
            {
                tokenType = squid::varNameToken;
            }
            else
            {
                tokenType = input[i].type;
            }
            tempTokens.push_back({tokenType, input[i].value, input[i].location});
        }
        std::cout << objectRef.size() << "\n";
        return tempTokens;
    }
    bool typeExists(std::string inputValue)
    {
        return typenameRef.count(inputValue);
    }

  private:
    std::set<std::string> typenameRef = {"int", "float", "double"};
    std::map<std::string, squid::object> objectRef;
};

class ramranch
{
  public:
    std::string returnType;
    enum expType
    {
        assignment,
        condition,
        funcDef,
        varDef,
        classDef
    } type;

    enum symbols
    {
        add,
        sub,
        mlp,
        div,
        mod,
        inc,
        deinc,
        plus,
        minus,
        lOr,
        lAnd,
        lNot,
        bOr,
        bAnd,
        bXor,
        bNot,
        shl,
        shr,
        assigns,
        equals,
        notEqual,
        lessThan,
        lessOrEqual,
        greaterThan,
        greaterOrEqual,
        pointer
    };

    struct objRef
    {
    };

    using expVec = std::vector<std::variant<ramranch, objRef, symbols, std::string>>;

    expVec exp;

    std::variant<ramranch, objRef, symbols, std::string> symbolMapper(squid::token input)
    {
        if (squid::scanner::isDigit(input.value))
        {
            return input.value;
        }
        else if (squid::utils::isInSS(squid::operatorTokens, input.value))
        {
            const std::string &sa = input.value; // Alias, name `sa` is arbitrary
            using syms = squid::ramranch::symbols;
            if (sa == "=")
            {
                return syms::assigns;
            }
            else if (sa == "<")
            {
                return syms::lessThan;
            }
            else if (sa == ">")
            {
                return syms::greaterThan;
            }
            else if (sa == "<=")
            {
                return syms::lessOrEqual;
            }
            else if (sa == ">=")
            {
                return syms::greaterOrEqual;
            }
            else if (sa == "*")
            {
                return syms::pointer;
            }
            else if (sa == "+")
            {
                return syms::add;
            }
            else if (sa == "-")
            {
                return syms::sub;
            }
            else if (sa == "/")
            {
                return syms::div;
            }
            else if (sa == "++")
            {
                return syms::inc;
            }
            else if (sa == "--")
            {
                return syms::deinc;
            }
            else if (sa == "==")
            {
                return syms::equals;
            }
            else if (sa == "!=")
            {
                return syms::notEqual;
            }
            else if (sa == "!")
            {
                return syms::lNot;
            }
            else if (sa == "~")
            {
                return syms::bNot;
            }
            else if (sa == "&&")
            {
                return syms::lAnd;
            }
            else if (sa == "&")
            {
                return syms::bAnd;
            }
            else if (sa == "|")
            {
                return syms::bOr;
            }
            else if (sa == "||")
            {
                return syms::lOr;
            }
            else if (sa == "^")
            {
                return syms::bXor;
            }
            else
            {
                return ""; // Should never get here, I just hate compiler warnings
            }
        }
        else
        {
            return ""; // Look 5 lines up
        }
    }

    bool isFinalBreakDown(expVec input)
    {
        unsigned char opCount = 0, valCount = 0;

        for (unsigned char i = 0; i < input.size(); i++)
        {
            if (opCount > 2 || valCount > 2)
            {
                return false;
            }
            else if (i == std::numeric_limits<unsigned char>::max())
            {
                // Error Source: 0x0020B, there should NEVER ~255 tokens that don't have atleast 2
                // operators or vars/lits

                std::wcerr << "Error: 0x0020B, something bad and unexpected happened. It's not "
                              "you, it's me";
            }

            if (input[i].index() == 2)
            {
                opCount++;
            }
            else if (input[i].index() != 3) // Per definition == 0 || 1
            {
                valCount++;
            }
            // Per `symbolMapper(...)` string type is trivial so no put in writing case
            // Writing this made me wonder if 
        }
    }

    ramranch(std::vector<squid::token> input)
    {
        mommy(input);
    }

    void mommy(std::vector<squid::token> input)
    {
        for (int i = 0; i < input.size(); i++)
        {
            exp.push_back(
                std::variant<ramranch, objRef, symbols, std::string>(symbolMapper(input[i])));
        }
    }
};

} // namespace squid

int main(int argc, char *argv[])
{

    squid::scanner mainScanner;
    squid::stage2_anal s2;

    std::ifstream source(argv[1]);
    std::string sourceString((std::istreambuf_iterator<char>(source)),
                             std::istreambuf_iterator<char>());

    mainScanner.analyze(sourceString);
    std::vector<squid::token> tokenListt = s2.analSex(mainScanner.fullTokens);
    std::vector<squid::token> tokenList(&tokenListt[7], &tokenListt[10]);

    std::cout << mainScanner.fullTokens.size();
    for (int i = 0; i < tokenList.size(); i++)
    {
        std::cout << tokenList[i].value << " : " << squid::tokenTypeToString(tokenList[i].type)
                  << '\n';
    }

    std::ofstream tokenJson;
    tokenJson.open("tokens.json");
    tokenJson << "[\n";

    for (int i = 0; i < tokenList.size(); i++)
    {
        if (tokenList[i].value != "\n")
        {
            tokenJson << fmt::format("{{\n\t\"type\": \"{}\",\n\t\"value\": \"{}\"\n}}{}\n",
                                     squid::tokenTypeToString(tokenList[i].type),
                                     tokenList[i].value, i + 1 == tokenList.size() ? "" : ",");
        }
    }
    tokenJson << "]";
    tokenJson.close();

    squid::ramranch expr(tokenList); // x = 2 + 2 should break into exp(x = exp(2 + 2))
}