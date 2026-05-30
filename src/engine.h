#pragma once
#include <iostream>
#include <string>
#include "window.h"

namespace lunaria
{
    class Engine 
    {
        public:

            Engine();
            void run();

            bool isClient = true;

            Window gameWindow;

            static std::string gameName;
            //static inline const char gameName[] = "lunaria test game";
    };
}
