#include <iostream>
#include <string>
#include "engine.h"


#include "resource/modelasset.h"
#include "resource/resourceman.h"

#include "window.h"

namespace lunaria
{
    std::string Engine::gameName = "lunaria test game";

    Engine::Engine()
    {
        std::cout << "Launching game" << std::endl;
    }

    Renderer* Engine::renderer;


    void Engine::run()
    {
        //window = {};
        Engine::renderer = window.GetRenderer();

        std::cout << "hi" << std::endl;
        resourceman.GetAsset<ModelAsset>("monkey.glb");

        window.loop();
    }
}
