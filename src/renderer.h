#pragma once
#include <vulkan/vulkan.h>

namespace lunaria
{
    class Renderer
    {
        public:
            Renderer();
            ~Renderer();

            VkSurfaceKHR surface;

        private:
            void InitRenderer();
            void Render();
    };
}