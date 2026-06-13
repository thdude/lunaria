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

        loop();
        //window.loop();
    }

    void Engine::loop()
    {
        math::Transform3D camera;
        camera.position = {-8, 0, 1};
        while(!glfwWindowShouldClose(window.window))
        {
            glfwPollEvents();
            
            renderer->Render(camera, 80, world.renderingSystem.GetRenderObjects());
            
            camera.position.x += 0.0001f;
        }
    }
}
