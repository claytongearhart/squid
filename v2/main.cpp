#include "terminal.hpp"
#include <algorithm>
#include <any>
#include <array>
#include <chrono>
#include <cstdio>
#include <deque>
#include <fcntl.h>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <numeric>
#include <random>
#include <regex>
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

bool isNumber(std::string input)
{
    return std::regex_match(input, std::regex("\\d*\\.?\\d+"));
}

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

// inline bool fileExist(const std::string &fn)
// {
//     return (stat(fn.c_str(), &buffer) == 0);
// }

// std::string randomString(size_t length)
// {
//     auto randchar = []() -> char {
//         const char charset[] = "0123456789"
//                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
//         const size_t maxIndex = (sizeof(charset) - 1);
//         return charset[rand() % maxIndex];
//     };
//     std::string str(length, 0);
//     std::generate_n(str.begin(), length, randchar);
//     return str;
// }

// std::string getFileName(std::string ending)
// {
//     std::string pfName = randomString(rand() % 32);
//     if (fileExist(pfName + ending))
//     {
//         return getFileName(ending);
//     }
//     return pfName + ending;
// }

unsigned long findNextNL(std::string &input, unsigned long startingPos)
{
    for (unsigned long i = startingPos; i < input.length(); i++)
    {
        if (input[i] == '\n')
        {
            return i;
        }
    }
    return input.size();
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

                             operatorTokens = {"<",  ">",  "<=", ">=", "*",  "+",  "-",
                                               "/",  "=",  "-=", "*=", "+=", "/=", "++",
                                               "--", "==", "&&", "||", "^"};
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
            else if (i + 1 == input.size())
            {
                return i + 1;
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

                if (beginTokenValue == "." ||
                    isDigit(std::to_string(beginTokenValue[0])) && isDigit(lastTokenValue))
                {
                    tempFullTokens.push_back(
                        {digitToken, beginTokenValue + lastTokenValue, fullTokens[i].location});
                }
                else
                {
                    tempFullTokens.push_back(
                        {tokenType(beginTokenValue), beginTokenValue, fullTokens[i].location});
                    tempFullTokens.push_back(
                        {tokenType(lastTokenValue), lastTokenValue, fullTokens[i].location});
                }
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

        while ((pos = find_first_of_delim(s, " []{}()<>+-*/&:;=^\n\"\0", lastPos)) !=
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
        // In here
        split(input);
        //
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

    std::vector<squid::token> format2(std::vector<token> input)
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
            if (exp.size() == 1)
            {
                root = btNode(exp[0]);
            }
            else
            {
                insert(exp);
            }
        }
    }

    void insert(std::vector<squid::token> exp)
    {
        std::vector<squid::token> postfixExp = toPostfix(exp);
        std::deque<binaryTreeNode> stack;
        squid::token chr = postfixExp[0];
        btNode node(chr);
        stack.push_back(node); // back -> left

        // for (int i = 0; i < postfixExp.size(); i++)
        // {
        //     std::cout << postfixExp[i].value << "\n";
        // }

        unsigned int i = 1;
        while (stack.size() > 0)
        {
            squid::token chr = postfixExp[i];

            if (chr.type == squid::operatorToken)
            {
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
                node = btNode(chr);
                stack.push_back(node);
            }

            i++;
            size++;

            if (i > 25)
            {
                // std::cout << "i > 25\n";
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
                                                     {"-", true},  {"=", false}};
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
        return ap > bp ? true : false;
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

// class function
// {
//     public:
//         std::string baseString;
//         std::vector<size_t> positions;
//         std::string compute(std::vector<std::string> args)
//         {
//             return fmt::format(baseString, args);
//         }
// };

// struct function
// {
//     std::vector<size_t> insertLocs;
//     std::list<squid::token> funcExp;
// };

class shell
{
  public:
    void loop()
    {
        while (true)
        {
            std::cout << "> ";
            std::string input;
            std::getline(std::cin, input);
            if (input == "clear")
            {
                std::cout << "\x1B[2J\x1B[H"; // *NIX only
            }
            else
            {
                std::cout << calc1(input) << "\n";
            }
        }
    }

  private:
    squid::scanner scan;
    squid::stage2_anal s2;
    std::map<std::string, double> varVals;
    // std::map<std::string, squid::function> funcDefs;

    size_t isFunction(std::vector<squid::token> &input, size_t start)
    {
        if (input[start + 1].value == "(")
        {
            size_t pScope = 1;
            unsigned short i = start + 2;
            while (i < input.size())
            {
                // std::cout << input[i].value << "\n";
                if (input[i].value == "(")
                {
                    pScope++;
                }
                else if (input[i].value == ")")
                {
                    pScope--;
                }
                i++;
                if (pScope == 0)
                {
                    return i;
                }
                if (i == std::numeric_limits<unsigned short>::max())
                {
                    std::cerr << "Error: Uneven parteneses\n";
                    return 0;
                }
            }
        }
        return 0;
    }

    std::optional<double> getConst(std::string input)
    {
        if (input == "pi")
        {
            return M_PI;
        }
        else if (input == "e")
        {
            return M_E;
        }
        return {};
    }

    double getVarVal(std::string vName)
    {
        if (getConst(vName).has_value())
        {
            return getConst(vName).value();
        }
        else if (varVals.find(vName) != varVals.end())
        {
            return varVals[vName];
        }
        else
        {
            // std::cout << "x=" << varVals["x"] << "\n";
            std::cerr << "Attempted use of undeclared variable: `" << vName << "`\n";
            loop();
            return 0.0;
        }
    }

    // std::pair<squid::token, size_t> calcUFunc(std::vector<squid::token> input)
    // {

    //     squid::function calledFunc;
    //     try {
    //         calledFunc = funcDefs.at(input[0].value);
    //     }
    //     catch(...)
    //     {
    //         std::cerr << "Attempted use of undeclared function: `" << input[0].value << "`.\n";
    //         loop();
    //         return {{}, 0}; // Should never get here
    //     }

    //     std::vector<squid::token> insertResult = {squid::token(squid::delimiterToken, "(", 0)};

    //     unsigned short pScope = 1;
    //     unsigned short i = 2;
    //     while (pScope > 0)
    //     {
    //             if (input[i].value == ")")
    //             {
    //                 pScope--;
    //             }
    //             else if (input[i].value == "(")
    //             {
    //                 pScope++;
    //             }
    //             insertResult.push_back(input[i]);
    //             i++;
    //     }
    //     insertResult.pop_back();
    //     insertResult.push_back(squid::token(squid::delimiterToken, ")", 0));

    //     for (unsigned short j = 0; j < calledFunc.insertLocs.size(); j++)
    //     {

    //     }
    // }

    std::pair<squid::token, size_t> calcFunction(std::vector<squid::token> input)
    {
        if (input[0].value == "sqrt")
        {
            //std::cout << input[input.size()].value << "\n";
            std::vector<squid::token> insertResult = {squid::token(squid::delimiterToken, "(", 0)};

            unsigned short pScope = 1;
            unsigned short i = 2;
            while (pScope > 0)
            {
                if (input[i].value == ")")
                {
                    pScope--;
                }
                else if (input[i].value == "(")
                {
                    pScope++;
                }
                insertResult.push_back(input[i]);
                i++;
            }
            insertResult.pop_back();
            insertResult.push_back(squid::token(squid::delimiterToken, ")", 0));

            // std::cout << insertResult.size() << "\n";

            std::string returnVString;
            // std::cout << "ir size " << insertResult.size() << "\n";
            if (insertResult.size() > 3)
            {
                returnVString = std::to_string(sqrt(std::stol(calc2(insertResult))));
            }
            else if (isNumber(insertResult[1].value))
            {
                returnVString = std::to_string(sqrt(std::stol(insertResult[1].value)));
            }
            else
            {
                // std::cout << "value:" << insertResult[1].value << "end\n";
                returnVString = std::to_string(sqrt(getVarVal(insertResult[1].value)));
            }

            return std::make_pair<squid::token, size_t>(
                squid::token(squid::digitToken, returnVString, 0), i - 1);
        }
        else
        {
            return std::make_pair<squid::token, size_t>(squid::token(squid::digitToken, "0", 0), 0);
        }
    }

    std::string calc2(std::vector<squid::token> input)
    {
        // if (input.size() == 1)
        // {
        //     // << "here\n";
        //     return input[0].value;
        // }
        squid::binaryExpressionTree tree(input);
        for (int i = 0; i < input.size(); i++)
        {
            std::cout << input[i].value << "\n";
        }
        scan.fullTokens.clear();
        if (tree.root.has_value())
        {
            if (tree.root.value().data.value == "=")
            {
                double varVal =
                    solver(std::any_cast<squid::binaryTreeNode>(tree.root.value().right));
                std::string varName =
                    std::any_cast<squid::binaryTreeNode>(tree.root.value().left).data.value;
                defHandler(varName, varVal);
                return "`" + varName + "` modified to " + std::to_string(varVal);
            }
            else
            {
                return std::to_string(solver(tree.root.value()));
            }
        }
        else
        {
            return "Error: Tree does not have root";
        }
    }
    std::string calc1(std::string in)
    {
        // std::cout << preprocess(in) << "\n";
        scan.analyze(preprocess(in) + " ");
        std::vector<squid::token> tokens = s2.format2(scan.fullTokens);
        std::vector<squid::token> tokens2;
        for (int i = 0; i < tokens.size(); i++)
        {

            if (size_t funcEnd = isFunction(tokens, i))
            {
                auto result = calcFunction(
                    std::vector<squid::token>(tokens.begin() + i, tokens.begin() + funcEnd));
                tokens2.push_back(result.first);
                i += result.second;
            }
            else
            {
                tokens2.push_back(tokens[i]);
            }
        }
        return calc2(tokens2);
    }
    void defHandler(std::string name, double value)
    {
        varVals[name] = value;
    }

    double solver(squid::binaryTreeNode input)
    {
        if (input.isLeaf())
        {
            if (isNumber(input.data.value))
            {
                return std::stod(input.data.value);
            }
            else
            {
                return getVarVal(input.data.value);
            }
        }
        else
        {
            squid::binaryTreeNode lr[] = {std::any_cast<squid::binaryTreeNode>(input.left),
                                          std::any_cast<squid::binaryTreeNode>(input.right)};

            if (input.data.value == "+")
            {
                return solver(lr[0]) + solver(lr[1]);
            }
            else if (input.data.value == "-")
            {
                return solver(lr[0]) - solver(lr[1]);
            }
            else if (input.data.value == "*")
            {
                return solver(lr[0]) * solver(lr[1]);
            }
            else if (input.data.value == "/")
            {
                return solver(lr[0]) / solver(lr[1]);
            }
            else if (input.data.value == "^")
            {
                return pow(solver(lr[0]), solver(lr[1]));
            }
            return 0.0;
        }
        return 0.0;
    }
};

} // namespace squid

int main(int argc, char *argv[])
{
    auto start = std::chrono::high_resolution_clock::now();
    // squid::scanner mainScanner;
    // squid::stage2_anal s2;

    // std::ifstream source(argv[1]);
    // std::string sourceString = preprocess(
    //     std::string((std::istreambuf_iterator<char>(source)), std::istreambuf_iterator<char>()));

    // // Problem Stems from in here
    // mainScanner.analyze(sourceString);
    // std::vector<squid::token> asd = mainScanner.fullTokens;
    // // End

    // for (int i = 0; i < asd.size(); i++)
    // {
    //     std::cout << asd[i].value << " : " << squid::tokenTypeToString(asd[i].type)
    //               << '\n';
    // }
    // std::vector<squid::token> tokenList(s2.analSex(mainScanner.fullTokens));

    // squid::binaryExpressionTree expTree(tokenList);
    // if (expTree.root.has_value())
    // {
    //     std::cout << solver(expTree.root.value()) << "\n";
    // }
    squid::shell mainShell;
    mainShell.loop();

    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> duration = end - start;
    // std::cout << "Compiled in " << duration.count() << " seconds\n";
}