#include <string>
#include <iostream>

namespace squid
{
    class console
    {
      public:
        void error(std::string errorMessage)
        {
            std::cerr << "Error: " << errorMessage << "\n";
        }  
    };
}