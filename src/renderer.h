#pragma once
#include <vulkan/vulkan.h>
#include <functional>

namespace lunaria
{
    class Renderer
    {
        public:
            Renderer(std::function<void(VkInstance,VkSurfaceKHR*)> bruh);
            ~Renderer();

            VkSurfaceKHR surface;

        private:
            void InitRenderer(std::function<void(VkInstance,VkSurfaceKHR*)> bruh);
            void Render();
    };
}