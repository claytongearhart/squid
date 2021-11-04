#include <vector>
#include <algorithm>
#include <utility>
#include <string>
#include "utils.hpp"
#include "errors.hpp"

namespace squid
{
    class word
    {
        public:
            std::string identifier;
        protected:
            std::vector<squid::word::any> children;
    };

    class objectCall
    {
        public:
        
    };

    namespace word
    {

        class type
        {
            
        };

        class any
        {
            public:

        };

        class variable : public squid::word
        {
            public:
                variable()
                {

                }
            private:
                void initialize()
                {

                }

                squid::error monitor()
                {
                    bool isErrorPresent;
                    squid::error returnObject();

                    if (!isErrorPresent)
                    {
                        returnObject.isThere = false;
                    }
                }

                bool isReference;
                variable referenceTo();
        };

        class function : public squid::word
        {
            public:
                type returnType;
                std::vector<std::pair<type, variable>> arguments;
        };

        class literal : public squid::word
        {

        };

        // Call Tokens
        
        class variableAccess : public squid::objectCall
        {
            variableAccess(literal input)
            {

            }
        };

        class functionCall : public squid::objectCall
        {
            public:
                std::vector<std::pair<type, variableAccess>> arguments;
        };
    }
}