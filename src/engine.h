#pragma once
#include "window.h"
#include "rendering/renderer.h"
#include "resource/resourceman.h"
#include "world/world.h"
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
            void loop();

            bool isClient = true;

            Window window;
            ResourceMan resourceman;
            World world;

            static Renderer* renderer;
            

            static std::string gameName;
            //static inline const char gameName[] = "lunaria test game";
    };
}
