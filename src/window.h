#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace lunaria
{
    class Window
    {
        public:
            Window();
            ~Window();

            void InitWindow();
        private:
            GLFWwindow *window;
            static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
            int width;
            int height;
            bool framebufferResized = false;

            void loop();
    };
}