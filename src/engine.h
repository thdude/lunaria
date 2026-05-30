#pragma once
#include "window.h"
#include "rendering/renderer.h"
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

            Window window;

            static Renderer* renderer;

            static std::string gameName;
            //static inline const char gameName[] = "lunaria test game";
    };
}
