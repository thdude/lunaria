#pragma once
#include <iostream>
#include <string>

namespace lunaria
{
    class Engine 
    {
        public:

            Engine();
            void run();

            bool isClient = true;

            //static std::string gameName;
            static inline const char gameName[] = "lunaria test game";
    };
}
