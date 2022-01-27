#include "terminal.hpp"
#include <algorithm>
#include <any>
#include <array>
#include <chrono>
#include <cstdio>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <numeric>
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

#define VERSION "0.2.1"
#define SQUID_BLUE "\033[1;38;5;19m>"

std::chrono::system_clock::time_point START_TIME;
bool isNumber(std::string input)
{
    return std::regex_match(input, std::regex("-?\\d*\\.?\\d+"));
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

std::string getSquidInfo()
{
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
                                               "--", "==", "&&", "||", "^",  "%"};
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

        while ((pos = find_first_of_delim(s, " []{}()<>+-*/&:;=^%\n\"\0", lastPos)) !=
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
    binaryTreeNode()
    {
    }

    bool isLeaf()
    {
        return !(left.has_value() || right.has_value());
    }
};

class binaryExpressionTree
{
    using btNode = binaryTreeNode;

  public:
    std::optional<btNode> root;
    std::string stringValue;
    unsigned int size;
    binaryExpressionTree(std::vector<squid::token> exp)
    {
        root = {};
        size = 0;

        //std::cout << "Exp size : " << exp.size() << " \n";

        for (unsigned int i = 0; i < exp.size(); i++)
        {
            stringValue += exp[0].value;
        }

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
        else
        {
                            std::cout << "Too small\n";
        }
    }

    binaryExpressionTree()
    {
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
            // std::cout << "l562\n";
            squid::token chr = postfixExp[i];

            if (chr.type == squid::operatorToken)
            {
                // std::cout << "l567\n";
                btNode operatorNode(chr);
                operatorNode.op = true;

                operatorNode.right = stack.back();
                stack.pop_back();
                // std::cout << "l573\n";

                if (stack.size() > 0)
                {
                    operatorNode.left = stack.back();
                    stack.pop_back(); // Stack is not big enough
                }

                // std::cout << "l578\n";

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
            {"=", 0},  {"||", 1},  {"&&", 2},  {"|", 3},  {"^", 11}, {"&", 5},  {"==", 6},
            {"!=", 6}, {"<", 7},   {">", 7},   {"<=", 7}, {">=", 7}, {"<<", 8}, {">>", 8},
            {"+", 9},  {"-", 9},   {"*", 10},  {"/", 10}, {"%", 10}, {"!", 11}, {"!", 11},
            {"~", 11}, {"++", 12}, {"--", 12}, {"::", 13}};
        return precedence[input];
    }
    bool isLR(std::string input)
    {
        std::map<std::string, bool> associativity = {{"++", true}, {"--", true}, {"*", true},
                                                     {"/", true},  {"%", true},  {"+", true},
                                                     {"-", true},  {"=", false}, {"^", true}};
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

class function
{
  public:
    std::string varName;
    squid::binaryExpressionTree expression;

    function(std::string name, squid::binaryExpressionTree exp)
    {
        varName = name;
        expression = exp;
    }
};

class shell
{
  public:
    void loop()
    {
        while (true)
        {
            std::cout << "\033[1;38;5;19m>\033[0m ";
            std::string input;
            std::getline(std::cin, input);
            processInput(input);
        }
    }

    void processInput(std::string input)
    {
        auto clockStart = std::chrono::high_resolution_clock::now();
        input += " ";
        std::vector<std::string> commands;
        std::string tempCommandString;
        for (size_t i = 0; i < input.size(); i++)
        {
            if (input[i] != ' ')
            {
                tempCommandString.push_back(input[i]);
            }
            else
            {
                commands.push_back(tempCommandString);
                tempCommandString = "";
            }
        }
        if (commands[0] == "")
        {
            std::cout << "Empty input provided\n";
        }
        else if (commands[0] == "clear")
        {
            std::cout << "\x1B[2J\x1B[H"; // *NIX only
        }
        else if (commands[0] == "help")
        {
            help();
        }
        else if (commands[0] == "list")
        {
            if (commands[1] == "functions")
            {
               for (auto it = functionDefs.begin(); it != functionDefs.end(); it++)
               {
                   std::cout << it->first << "\n";
               } 
            }
        }
        else if (commands[0] == "dump")
        {
            if (commands[1] == "function")
            {
                squid::function gotFunc = functionDefs.at(commands[2]);
        
                std::cout << "Function name " << gotFunc.varName << "\nFunction expresion value " << "NA" << '\n';
            }
        }
        else if (commands[0] == "quit")
        {
            auto timet = std::chrono::hh_mm_ss<std::chrono::nanoseconds>(
                std::chrono::system_clock::now() - START_TIME);
            std::cout << "Session lasted "
                      << (timet.hours().count() > 0 ? std::to_string(timet.hours().count()) + "h "
                                                    : "")
                      << (timet.minutes().count() > 0
                              ? std::to_string(timet.minutes().count()) + "m "
                              : "")
                      << timet.seconds().count() << "s\nNow quitting\n";
            std::exit(0);
        }
        else
        {
            std::cout << calc1(input) << "\n";
        }
    }

    squid::scanner scan;
    squid::stage2_anal s2;
    std::map<std::string, double> varVals = {{"", 0}};
    std::map<std::string, squid::function> functionDefs;
    std::list<std::string> commandsList;
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
                    //std::cout << i << '\n';
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

    double getVarVal(std::string vName, std::map<std::string, double> vvInput)
    {
        if (getConst(vName).has_value())
        {
            return getConst(vName).value();
        }
        else if (vvInput.find(vName) != vvInput.end())
        {
            return vvInput[vName];
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

    void help()
    {
        std::cout
            << "\x1B[2J\x1B[HBasic Arithmetic:\n\tType any expression.\n\tEx. "
               "2+2\nVariables:\n\tVariables are defined using the `=` character\n\tEx. x=2, "
               "y=2+2\nFunctions:\n\tSo far custom functions aren't suppported,\n\tbut you can use "
               "the functions sqrt, sin, cos, & tan\n\tEx. sin(2+2), cos(sin(pi/2))\n";
    }

    std::pair<squid::token, size_t> compSingleValFunc(std::vector<squid::token> input,
                                                      double (*func)(double))
    {
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
            // std::cout << calc2(insertResult) << "l861\n";
            returnVString = std::to_string(func(std::stold(calc2(insertResult))));
            // std::cout << returnVString << "\n";
        }
        else if (isNumber(insertResult[1].value))
        {
            // std::cout << ";866\n";
            returnVString = std::to_string(func(std::stold(calc2(insertResult))));
            // std::cout << returnVString << "\n";
        }
        else
        {
            // std::cout << "value:" << insertResult[1].value << "end\n";
            returnVString = std::to_string(func(getVarVal(insertResult[1].value, varVals)));
        }

        return std::make_pair<squid::token, size_t>(
            squid::token(squid::digitToken, returnVString, 0), i - 1);
    }

    std::pair<squid::token, size_t> calcFunction(std::vector<squid::token> input)
    {
        //std::cout << "l955\n";
        std::string *const fn = &input[0].value; // fn = function name
        if (*fn == "sqrt")
        {
            return compSingleValFunc(input, sqrt);
        }
        else if (*fn == "sin")
        {
            return compSingleValFunc(input, sin);
        }
        else if (*fn == "cos")
        {
            return compSingleValFunc(input, cos);
        }
        else if (*fn == "tan")
        {
            return compSingleValFunc(input, tan);
        }
        else if (*fn == "ln")
        {
            return compSingleValFunc(input, log);
        }
        else if (functionDefs.count(*fn) > 0)
        {
            // std::map<std::string, double> funcVarMap;
            // funcVarMap[functionDefs[*fn].varName] = std::stod(input[2].value);
            //std::cout << "l979\n";
            std::map<std::string, double> combinedVarVals(varVals);

            combinedVarVals[functionDefs.at(*fn).varName] = std::stod(input[2].value); // This works

            std::cout << "String value  " << functionDefs.at(*fn).expression.stringValue << "\n";

            return std::make_pair<squid::token, size_t>(
                squid::token(squid::digitToken,
                             std::to_string(solver(functionDefs.at(*fn).expression.root.value(),
                                                   combinedVarVals)),
                             0),
                0);
        }
        else
        {
            //std::cout << *fn << " " << functionDefs.count(*fn) << "\n";
            return std::make_pair<squid::token, size_t>(squid::token(squid::digitToken, "0", 0), 0);
        }
    }

    bool isFuncDef(std::vector<squid::token> input, size_t decEnd)
    {

        //return false; // Not working but don't want to delete code

        if (input[0].value == "=")
        {

            //std::cout << input[decEnd - 1].value << "\n";
            return true;
        }
        else
        {
            return false;
        }
    }

    double compFunc(squid::binaryExpressionTree, std::map<std::string, double> vars)
    {
    }

    std::string calc2(std::vector<squid::token> tokens)
    {
        // std::cout << "l911\n";
        if (tokens[0].value == "(" && tokens[tokens.size() - 1].value == ")")
        {
            // std::cout << "l911\n";
            tokens.erase(tokens.begin());
            tokens.pop_back();
        }
        // std::cout << tokens[0].value << " " << tokens[tokens.size()].value <<" l915\n";

        std::vector<squid::token> tokens2;
        for (int i = 0; i < tokens.size(); i++)
        {
            /// std::cout << tokens[i].value << "l918\n";
            if (size_t funcEnd = isFunction(tokens, i))
            {
                auto funcDecTokCheck = std::vector<squid::token>(tokens.begin() + funcEnd , tokens.end());
                if (isFuncDef(funcDecTokCheck, tokens.size()))
                {
                    // Support for user defined functions, not working, won't return to normal loop properly
                    std::cout << "l1042 funcDecTokCheck size : " << funcDecTokCheck.size() << "\n";
                    for (int i = 1; i < funcDecTokCheck.size(); i++)
                    {
                        std::cout << "Value :" << funcDecTokCheck[i].value << "\n";
                    }

                    function insertFunc(tokens[i].value,
                                        squid::binaryExpressionTree(std::vector<squid::token>(funcDecTokCheck.begin() + 1, funcDecTokCheck.end())));
                    //std::cout << "Function name: " << tokens[i].value << " \n";
                    functionDefs.insert(std::pair<std::string, squid::function> (tokens[i].value, insertFunc));
                    i = tokens.size();
                    break;
                }
                else
                {
                    auto result = calcFunction(
                        std::vector<squid::token>(tokens.begin() + i, tokens.begin() + funcEnd));
                    tokens2.push_back(result.first);
                    i += result.second;
               }
            }
            else
            {
                tokens2.push_back(tokens[i]);
            }
        }
        squid::binaryExpressionTree tree(tokens2);
        // std::cout << "l939\n";
        scan.fullTokens.clear();
        if (tree.root.has_value())
        {
            if (tree.root.value().data.value == "=")
            {
                double varVal =
                    solver(std::any_cast<squid::binaryTreeNode>(tree.root.value().right), varVals);
                std::string varName =
                    std::any_cast<squid::binaryTreeNode>(tree.root.value().left).data.value;
                defHandler(varName, varVal);
                return "`" + varName + "` modified to " + std::to_string(varVal);
            }
            else
            {
                return std::to_string(solver(tree.root.value(), varVals));
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
        commandsList.push_back(in);
        scan.analyze(preprocess(in) + " ");
        std::vector<squid::token> tokens = s2.format2(scan.fullTokens);

        return calc2(tokens);
    }
    void defHandler(std::string name, double value)
    {
        varVals[name] = value;
    }

    double solver(squid::binaryTreeNode input, std::map<std::string, double> varValues)
    {
        if (input.isLeaf())
        {
            if (isNumber(input.data.value))
            {
                return std::stod(input.data.value);
            }
            else
            {
                return getVarVal(input.data.value, varValues);
            }
        }
        else
        {
            squid::binaryTreeNode lr[] = {
                input.left.has_value() ? std::any_cast<squid::binaryTreeNode>(input.left)
                                       : squid::binaryTreeNode(),
                input.right.has_value() ? std::any_cast<squid::binaryTreeNode>(input.right)
                                        : squid::binaryTreeNode()}; // Not very safe but oh well

            std::string &idv = input.data.value;

            if (idv == "+")
            {
                return solver(lr[0], varValues) + solver(lr[1], varValues);
            }
            else if (idv == "-")
            {
                return solver(lr[0], varValues) - solver(lr[1], varValues);
            }
            else if (idv == "*")
            {
                return solver(lr[0], varValues) * solver(lr[1], varValues);
            }
            else if (idv == "/")
            {
                return solver(lr[0], varValues) / solver(lr[1], varValues);
            }
            else if (idv == "^")
            {
                return pow(solver(lr[0], varValues), solver(lr[1], varValues));
            }
            else if (idv == "%")
            {
                return (long)solver(lr[0], varValues) % (long)solver(lr[1], varValues);
            }
            else if (idv == "++")
            {
                return solver(lr[1], varValues) + 1;
            }
            else if (idv == "==")
            {
                return solver(lr[0], varValues) == solver(lr[1], varValues);
            }
            return 0.0;
        }
        return 0.0;
    }
};

} // namespace squid

int main(int argc, char *argv[])
{
    // auto start = std::chrono::high_resolution_clock::now();
    //  squid::scanner mainScanner;
    //  squid::stage2_anal s2;

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
    START_TIME = std::chrono::system_clock::now();

    squid::shell mainShell;

    if (argc > 1)
    {
        if (std::string(argv[1]) == "-d")
        {
            std::cout << "\033[1;38;5;19mSquid " << VERSION
                      << " DEV\033[0m\nType `help` for more details\n";
            mainShell.loop();
        }
        else
        {
            std::string args =
                argc > 1 ? std::accumulate(argv + 2, argv + argc, std::string(argv[1]),
                                           [](auto &&lhs, auto &&rhs) {
                                               return std::forward<decltype(lhs)>(lhs) + rhs;
                                           })
                         : "";
            std::cout << mainShell.calc1(args) << "\n";
        }
    }
    else
    {
        std::cout << "\033[1;38;5;19mSquid 0.2.1\033[0m\nType `help` for more details\n";

        mainShell.loop();
    }
    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> duration = end - start;
    // std::cout << "Compiled in " << duration.count() << " seconds\n";

    return 0;
}