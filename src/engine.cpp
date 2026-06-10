#include <iostream>
#include <string>
#include "engine.h"

#include "resource/asset.h"
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


    void Engine::run()
    {
        window = {};
        Engine::renderer = window.GetRenderer();

        resourceman = {};
        resourceman.GetAsset<ModelAsset>("monkey.glb");
    }
}
