#include <vector>
#include <algorithm>
#include <string>
#include "utils.hpp"

namespace squid
{
    class word
    {
        public:
            std::string identifier;
        protected:
            std::vector<squid::word::any> children;
    };

    namespace word
    {
        class types
        {
            public:

                types(std::vector<squid::token> input)
                {
                    if(std::find(std::begin(literalTypes), std::end(literalTypes), input.back().value) == std::end(literalTypes));
                    {
                        // Throw error
                    }
                }

                

                enum baseTypes 
                {
                    integer,
                    decimal,
                    character
                } baseType;

                enum signage: bool
                {
                    signed_m,
                    unsigned_m
                };

                bool isArray;

                unsigned short size;

            private:
                                const std::string literalTypes[4] = {"string", "char", "int", "float"};
        };


        class any
        {
            public:

        };

        class variable : public squid::word
        {

        };

        class function : public squid::word
        {

        };

        class literal : public squid::word
        {

        };
    }
}