#include <vulkan/vulkan.h>

namespace lunaria
{
    class Renderer
    {
        public:
            Renderer();
            ~Renderer();

        private:
            VkInstance instance;

            void InitRenderer();
            void Render();
            void createInstance();
            void findDevice();
            int rateDevice(VkPhysicalDevice device);
    };
}