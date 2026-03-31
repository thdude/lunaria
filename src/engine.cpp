#include <iostream>
#include <string>
#include "engine.h"

#include "window.h"

namespace lunaria
{
    std::string Engine::gameName = "lunaria test game";

    Engine::Engine()
    {
        std::cout << "Launching game" << std::endl;
    }


    void Engine::run()
    {
        lunaria::Window window{};
    }
}
