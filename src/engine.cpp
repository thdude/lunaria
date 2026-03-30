#include <iostream>
#include <string>
#include "engine.h"

#include "window.h"

namespace lunaria
{
    Engine::Engine()
    {
        //gameName = "lunaria test game";

        std::cout << "Launching game" << std::endl;
    }


    void Engine::run()
    {
        lunaria::Window window{};
    }
}
