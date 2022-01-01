#include <algorithm>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>
#include <math.h>


unsigned long findNextNL(std::string &input, unsigned long startingPos)
{
    for (unsigned long i = startingPos; i < input.length(); i++)
    {
        if (input[i] == '\n')
        {
            return i; 
        }
    }
    throw std::out_of_range("Source code cannot end with comment");
}

std::string preprocess(std::string input)
{
    std::string rString;
    for (unsigned long i = 0; i < input.length(); i++)
    {
        if (input[i] == '/' && input[i + 1] == '/')
        {
            i = findNextNL(input, i);
        }
        else if (input[i] == '\n' || input[i] == '\t')
        {
            rString += ' ';
        }
        else
        {
            rString += input[i];
        }

        
    }

    rString.erase(std::unique(std::begin(rString), std::end(rString), [](unsigned char a, unsigned char b){
        return std::isspace(a) && std::isspace(b);
    }), std::end(rString));

    return rString;
}

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
    typeToken,
    varNameToken,
    funcDefToken,
    funcCallToken,
    other
};

class token
{
  public:
    tokenTypes type;
    size_t location;
    std::string value;


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

class scanner
{
  private:
    std::vector<std::pair<int, int>> stringLocations;
    std::vector<std::string> boolTokens = {"true", "false"},

                             delimiterTokens = {"(", ")", "{", "}", ";", ","},

                             keywordTokens = {"auto", "do", "switch", "return", "class"},

                             typeTokens = {"int", "float", "double"},

                             operatorTokens = {"<", ">",  "<=", ">=", "*",  "+",  "-",  "/",
                                               "=", "-=", "*=", "+=", "/=", "++", "--", "==", "&&", "||"};
    std::vector<std::pair<std::string, int>> tokenValues;
    bool isDigit(const std::string &input)
    {
        return std::all_of(input.begin(), input.end(), ::isdigit);
    }

    bool isString(const std::string &input)
    {
        return input[0] == '\"' && input[input.size() - 1] == '\"';
    }
    void findStrings(std::string input)
    {
        std::vector<int> quoteLocations;
        std::vector<int> dQuoteLocations;

        for (int i = 0; i < input.size() + 1; i++)
        {
            bool isEscaped = input[i] == '\\';

            if (input[i + 1] == '\"' && !isEscaped)
            {
                dQuoteLocations.push_back(i + 1);
            }
            else if (input[i + 1] == '\'' && !isEscaped)
            {
                quoteLocations.push_back(i + 1);
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

        return squid::utils::isInStringVec(operatorTokens, token)    ? squid::operatorToken
               : squid::utils::isInStringVec(delimiterTokens, token) ? squid::delimiterToken
               : squid::utils::isInStringVec(keywordTokens, token)   ? squid::keywordToken
               : squid::utils::isInStringVec(boolTokens, token)      ? squid::boolToken
               : squid::utils::isInStringVec(typeTokens, token)      ? squid::typeToken
               : isString(token)                                     ? squid::stringToken
               : isDigit(token)                                      ? squid::digitToken
                                                                     : squid::other;
    }

    // Tokens would have reminant of last token on current token if size > 1 so this removes it
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

        std::string operatorDelims = "<>:=+-&|";
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

    void split(std::string s)
    {
        size_t pos = 0, lastPos = 0;

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
    std::set<std::string> typenameRef = {"int", "float", "double"};

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
            else if (((input[i - 1].type == squid::tokenTypes::typeToken) && // This top eval is responsible for the seg faults
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
                std::cout << squid::tokenTypeToString(input[i - 1].type) << '\n';
                tokenType = input[i].type;
            }

            tempTokens.push_back({tokenType, input[i].value, input[i].location});
        }
                return tempTokens;


    }
    bool typeExists(std::string inputValue)
    {
        return typenameRef.count(inputValue);
    }

  private:
    std::map<std::string, squid::object> objectRef;
};

} // namespace squid

int main(int argc, char *argv[])
{
    squid::scanner mainScanner;
    squid::stage2_anal s2;


    std::ifstream source(argv[1]);
    std::string sourceString = preprocess(std::string((std::istreambuf_iterator<char>(source)),
                             std::istreambuf_iterator<char>()));

    std::cout << sourceString << '\n';

    mainScanner.analyze(sourceString);
    std::vector<squid::token> tokenList(s2.analSex(mainScanner.fullTokens));


    for (int i = 0; i < tokenList.size(); i++)
    {
        std::cout << tokenList[i].value << " : " << squid::tokenTypeToString(tokenList[i].type)
                  << '\n';
    }

    std::ofstream tokenJson;
    tokenJson.open("tokens.json");
    tokenJson << "{\n\"tokens\": [\n"; 

    for (int i = 0; i < tokenList.size(); i++)
    {
        if (tokenList[i].value != "\n")
        {
        tokenJson << fmt::format("{{\n\t\"type\": \"{}\",\n\t\"value\": \"{}\"\n}}{}\n",
                                 squid::tokenTypeToString(tokenList[i].type), tokenList[i].value,
                                 i + 1 == tokenList.size() ? "" : ",");
        }
    }
    tokenJson << "],\n\"typenames\":\n[\n";
    for (std::set<std::string>::iterator it = s2.typenameRef.begin(); it != s2.typenameRef.end();
         it++)
    {
        tokenJson << "\t\"" << *it << "\"" << (std::distance(s2.typenameRef.begin(), it) == s2.typenameRef.size() - 1 ? "" : ",") << "\n";

    }
    tokenJson << "]}";
    tokenJson.close();
}