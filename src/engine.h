#pragma once
#include "window.h"
#include "rendering/renderer.h"
#include "resource/resourceman.h"
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

            Window window;
            ResourceMan resourceman;

            static Renderer* renderer;
            

            static std::string gameName;
            //static inline const char gameName[] = "lunaria test game";
    };
}
