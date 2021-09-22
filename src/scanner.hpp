#include "utils.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class scanner
{
  private:
    std::vector<std::pair<int, int>> stringLocations;
    std::vector<std::string>
        boolTokens = {"true", "false"},

        delimiterTokens = {"(", ")", "{", "}", ";", ","},

        keywordTokens = {"int", "float",  "auto",  "double",
                         "do",  "switch", "return"},

        operatorTokens = {"<", ">",  "<=", ">=", "*",  "+",  "-",  "/",
                          "=", "-=", "*=", "+=", "/=", "++", "--", "=="};
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

        for (int i = 0; i < input.size(); i++)
        {
            if (input[i] == '\"' ||
                input[i] == '\'' && input[i - 1] != '\\')
            {
                quoteLocations.push_back(i);
            }
        }
        for (int i = 0; i < quoteLocations.size(); i += 2)
        {
            stringLocations.emplace_back(
                std::make_pair(quoteLocations[i], quoteLocations[i + 1]));
        }
    }

    bool isInString(int location)
    {
        for (int i = 0; i < stringLocations.size(); i++)
        {
            if (stringLocations[i].first < location &&
                location < stringLocations[i].second)
            {
                return true;
            }
        }
        return false;
    }

    size_t find_first_of_delim(std::string input, std::string delims, size_t lastPos)
    {
        std::string delims = " []{}()<>+-*/&:.\n\"\'";

        for (int j = 0; j < delims.size(); j++)
        {
            if (delims[j] != c)
            {
                return false;
            }
        }

        if (isInString(location))
        {
            return false;
        }

        return true;
    }

    // void fixStrings()
    // {
    //     std::vector<squid::token> tempTokenList;
    //     findStrings();

    //     // for (int i = 0; i < fullTokens.size(); i++)
    //     // {
    //     //     if (isInString(i))
    //     //     {
    //     //         std::string newToken =
    //     //             fullTokens[i].value + fullTokens[i + 1].value;
    //     //         tempTokenList.push_back({squid::stringToken,
    //     newToken,
    //     //                                  fullTokens[i].location});
    //     //         i++;
    //     //     }
    //     //     else
    //     //     {
    //     // tempTokenList.push_back({tokenType(fullTokens[i].value),
    //     //                                  fullTokens[i].value,
    //     //                                  fullTokens[i].location});
    //     //     }
    //     // }
    //     std::stringstream ss;
    //     std::vector <std::string> stringVec;
    //     for (int i = 0; i < stringLocations.size(); i++)
    //     {
    //         std::for_each(
    //             fullTokens.begin() + stringLocations[i].first,
    //             fullTokens.begin() + stringLocations[i].second,
    //             [&i, &stringVec](const std::string &s) { stringVec[i] +=
    //             s; });
    //     }

    //     fullTokens = tempTokenList;
    // }

    squid::tokenTypes tokenType(std::string token)
    {

        return squid::utils::isInStringVec(operatorTokens, token)
                   ? squid::boolToken
               : squid::utils::isInStringVec(delimiterTokens, token)
                   ? squid::delimiterToken
               : squid::utils::isInStringVec(keywordTokens, token)
                   ? squid::keywordToken
               : squid::utils::isInStringVec(boolTokens, token)
                   ? squid::operatorToken
               : isString(token) ? squid::stringToken
               : isDigit(token)  ? squid::digitToken
                                 : squid::other;
    }

    void refineTokens()
    {
        std::vector<squid::token> tempFullTokens;
        for (int i = 0; i < fullTokens.size(); i++)
        {
            if (!(fullTokens[i].value.length() == 1))
            {
                auto beginTokenValue = fullTokens[i].value.substr(
                    0, fullTokens[i].value.size() - 1);
                std::string lastTokenValue(1, fullTokens[i].value.back());

                tempFullTokens.push_back({tokenType(beginTokenValue),
                                          beginTokenValue,
                                          fullTokens[i].location});
                tempFullTokens.push_back({tokenType(lastTokenValue),
                                          lastTokenValue,
                                          fullTokens[i].location});
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

    std::vector<std::pair<std::string, int>> split(std::string s)
    {
        std::vector<std::pair<std::string, int>> values;
        size_t pos = 0, lastPos = 0;
        while ((pos = s.find_first_of(
                    " []{}()<>+-*/&:.\n\"",
                    lastPos)) != // Make wrapper function of find_first_of
                                 // to check if is in string or not
               std::string::npos && !isInString(pos))
        {
            if (!isInString(lastPos))
            {
                values.emplace_back(std::make_pair(
                    s.substr(lastPos, pos - lastPos + 1), pos));
            }
            lastPos = pos + 1;
        }
        values.emplace_back(std::make_pair(s.substr(lastPos), pos));

        return values;
    }
    void analyze(std::string input)
    {
        tokenValues = split(input);
        calcTypes();
        refineTokens();
        findStrings(input);
        // sanatizeTokens();
        // findStrings();
    }

    void calcTypes()
    {
        for (int i = 0; i < tokenValues.size(); i++)
        {
            fullTokens.push_back({tokenType(tokenValues[i].first),
                                  tokenValues[i].first, 1});
        }
    }

    void sanatizeTokens()
    {
        for (int i = 0; i < fullTokens.size(); i++)
        {
            for (int j = 0; j < fullTokens[i].value.size(); j++)
            {
                if (squid::utils::isSpace(fullTokens[i].value[j])) //
                {
                    fullTokens[i].value.erase(j, 1);
                }
            }
            if (fullTokens[i].value == "")
            {
                fullTokens.erase(fullTokens.begin() + i);
            }
        }
    }
};
