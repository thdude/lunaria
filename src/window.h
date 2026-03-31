#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "rendering/renderer.h"
#include <string>

namespace lunaria
{
    class Window
    {
        public: 
            Window();
            ~Window();

            void InitWindow();
            GLFWwindow *window;

        private:
            static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
            int width;
            int height;
            bool framebufferResized = false;

            Renderer *renderer;

            void loop();
    };
}