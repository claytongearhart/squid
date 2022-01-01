#include <algorithm>
#include <any>
#include <array>
#include <chrono>
#include <cstdio>
#include <deque>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <math.h>
#include <memory>
#include <numeric>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <sys/stat.h>
#include <utility>
#include <variant>
#include <vector>

std::string exec(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

inline bool fileExist(const std::string &fn)
{
    struct stat buffer;
    return (stat(fn.c_str(), &buffer) == 0);
}

std::string randomString(size_t length)
{
    auto randchar = []() -> char {
        const char charset[] = "0123456789"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const size_t maxIndex = (sizeof(charset) - 1);
        return charset[rand() % maxIndex];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

std::string getFileName(std::string ending)
{
    std::string pfName = randomString(rand() % 32);
    if (fileExist(pfName + ending))
    {
        return getFileName(ending);
    }
    return pfName + ending;
}

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

    rString.erase(std::unique(std::begin(rString), std::end(rString),
                              [](unsigned char a, unsigned char b) {
                                  return std::isspace(a) && std::isspace(b);
                              }),
                  std::end(rString));

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
    token()
    {
    }

    token(const token &inv)
    {
        type = inv.type;
        location = inv.location;
        value = inv.value;
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
    case funcDefToken:
        return "Function Definition Token";
    case funcCallToken:
        return "Function Call Token";
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

                             operatorTokens = {
                                 "<",  ">",  "<=", ">=", "*",  "+",  "-",  "/",  "=",
                                 "-=", "*=", "+=", "/=", "++", "--", "==", "&&", "||"};
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
            else if (((input[i - 1].type ==
                       squid::tokenTypes::typeToken) && // This top eval is responsible for the seg
                                                        // faults
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
        return tempTokens;
    }
    bool typeExists(std::string inputValue)
    {
        return typenameRef.count(inputValue);
    }

  private:
    std::map<std::string, squid::object> objectRef;
};
class binaryTreeNode
{
  public:
    squid::token data;
    std::any left, right;
    bool op;
    binaryTreeNode(squid::token in)
    {
        data = in;
        op = false;
    }

    bool isLeaf()
    {
        return !(left.has_value() && right.has_value());
    }
};

class binaryExpressionTree
{
    using btNode = binaryTreeNode;

  public:
    std::optional<btNode> root;
    unsigned int size;
    binaryExpressionTree(std::vector<squid::token> exp)
    {
        root = {};
        size = 0;

        if (!exp.empty())
        {
            insert(exp);
        }
    }

    void insert(std::vector<squid::token> exp)
    {
        std::vector<squid::token> postfixExp = toPostfix(exp);
        // for (ushort i = 0; i < postfixExp.size(); i++)
        // {
        //     std::cout << postfixExp[i].value << "\n";
        // }
        std::deque<binaryTreeNode> stack;
        squid::token chr = postfixExp[0];
        btNode node(chr);
        stack.push_back(node); // back -> left

        unsigned int i = 0;
        while (stack.size() != 0)
        {
            squid::token chr = postfixExp[i];

            std::cout << squid::tokenTypeToString(postfixExp[i].type) << "\n";
            if (chr.type == squid::operatorToken)
            {
                std::cout << "l536\n";
                btNode operatorNode(chr);
                operatorNode.op = true;

                operatorNode.right = stack.back();
                stack.pop_back();

                operatorNode.left = stack.back();
                stack.pop_back(); // Stack is not big enough

                stack.push_back(operatorNode);

                if (stack.size() == 1 && i == postfixExp.size() - 1)
                {
                    root = stack.back();
                    stack.pop_back();
                }
            }
            else
            {
                std::cout << "l556\n";
                node = btNode(chr);
                stack.push_back(node);
            }

            i++;
            size++;

            if (i > 25)
            {
                break;
            }
        }
    }

    unsigned int getPrec(std::string input)
    {
        std::map<std::string, unsigned int> precedence = {
            {"=", 0},  {"||", 1},  {"&&", 2},  {"|", 3},  {"^", 4},  {"&", 5},  {"==", 6},
            {"!=", 6}, {"<", 7},   {">", 7},   {"<=", 7}, {">=", 7}, {"<<", 8}, {">>", 8},
            {"+", 9},  {"-", 9},   {"*", 10},  {"/", 10}, {"%", 10}, {"!", 11}, {"!", 11},
            {"~", 11}, {"++", 12}, {"--", 12}, {"::", 13}};
        return precedence[input];
    }
    bool isLR(std::string input)
    {
        std::map<std::string, bool> associativity = {{"++", true}, {"--", true}, {"*", true},
                                                     {"/", true},  {"%", true},  {"+", true},
                                                     {"-", true}};
        return associativity[input];
    }

    bool higherPrec(squid::token a, squid::token b)
    {
        unsigned short ap = getPrec(a.value);
        unsigned short bp = getPrec(b.value);

        if (ap == bp)
        {
            if (isLR(a.value))
                return true;
            else
                return false;
        }
    }

    std::vector<squid::token> toPostfix(std::vector<squid::token> infix) // It's you
    {
        std::stack<squid::token> stack;
        std::vector<squid::token> postfix;

        for (unsigned int i = 0; i < infix.size(); i++)
        {
                        if (infix[i].type == squid::operatorToken)
            {
                while (!stack.empty() && stack.top().value != "(" &&
                       higherPrec(stack.top(), infix[i]))
                {

                    postfix.push_back(stack.top());
                    stack.pop();
                }
                stack.push(infix[i]);
            }
            else if (infix[i].value == "(")
            {
                stack.push(infix[i]);
            }
            else if (infix[i].value == ")")
            {
                while (!stack.empty() && stack.top().value != "(")
                {
                    postfix.push_back(stack.top());
                    stack.pop();
                }

                stack.pop();
            }
            else
            {
                postfix.push_back(infix[i]);
            }
        }
        while (!stack.empty())
        {
            postfix.push_back(stack.top());
            stack.pop();
        }


        return postfix;
    }
};

class solver
{
    
};

} // namespace squid

int main(int argc, char *argv[])
{
    auto start = std::chrono::high_resolution_clock::now();
    squid::scanner mainScanner;
    squid::stage2_anal s2;

    std::ifstream source(argv[1]);
    std::string sourceString = preprocess(
        std::string((std::istreambuf_iterator<char>(source)), std::istreambuf_iterator<char>()));

    mainScanner.analyze(sourceString);
    std::vector<squid::token> tokenList(s2.analSex(mainScanner.fullTokens));

    for (int i = 0; i < tokenList.size(); i++)
    {
        std::cout << tokenList[i].value << " : " << squid::tokenTypeToString(tokenList[i].type)
                  << '\n';
    }

    // std::ofstream tokenJson;
    // std::string fileName = getFileName(".json");
    // tokenJson.open(fileName);
    // tokenJson << "{\n\"tokens\": [\n";

    // for (int i = 0; i < tokenList.size(); i++)
    // {
    //     if (tokenList[i].value != "\n")
    //     {
    //         tokenJson << fmt::format("{{\n\t\"type\": \"{}\",\n\t\"value\": \"{}\"\n}}{}\n",
    //                                  squid::tokenTypeToString(tokenList[i].type),
    //                                  tokenList[i].value, i + 1 == tokenList.size() ? "" : ",");
    //     }
    // }
    // tokenJson << "],\n\"typenames\":\n[\n";
    // for (std::set<std::string>::iterator it = s2.typenameRef.begin(); it != s2.typenameRef.end();
    //      it++)
    // {
    //     tokenJson << "\t\"" << *it << "\""
    //               << (std::distance(s2.typenameRef.begin(), it) == s2.typenameRef.size() - 1 ? ""
    //                                                                                          :
    //                                                                                          ",")
    //               << "\n";
    // }
    // tokenJson << "]}";
    // tokenJson.close();
    // std::string cCode = exec(("python3 main.py " + fileName).c_str());
    // std::filesystem::remove(fileName);
 
    // std::ofstream cOut;
    // std::string cFN = getFileName(".c");
    // cOut.open(cFN);
    // cOut << cCode;

    tokenList.pop_back();
    tokenList.pop_back();
    squid::binaryExpressionTree expTree(tokenList);
    if (expTree.root.has_value())
    {
        std::cout << expTree.root.value().data.value << "\n";
    }
    else
    {
        std::cout << expTree.root.has_value() << "\n";
    }
    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> duration = end - start;
    // std::cout << "Compiled in " << duration.count() << " seconds\n";
}