#include <iostream>
#include <vector>

class token
{
    public:
            std::string type;
        std::string value;
        token(std::string t, char v)
        {
            type = t;
            value = v;
        }


};

std::vector<token> parseTokens(std::string &input)
{
    std::vector<token> rValue;
    for (int i = 0; i < input.size(); i++)
    {
        if (input[i] == '+')
        {
            rValue.push_back(token("op", input[i]));
        }
        else if (input[i] == ' ')
        {

        }
        else
        {
            rValue.push_back(token("num", input[i]));
        }
    }

    return rValue;
}

int main()
{
    std::string expression;
    std::cin >> expression;

    std::cout << expression;
}