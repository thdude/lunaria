#pragma once
#include <volk.h>
#include "utils/math.h"
#include <vk_mem_alloc.h>
#include <functional>
#include <string>
#include <iostream>

namespace lunaria
{
    struct framedata{

        int hello = 1;
        VkBuffer shaderData;
        VmaAllocation shaderDataAllocation;
        VkDeviceAddress shaderDataAddress;
        VkFence fence;
        VkSemaphore presentSemaphore;
        VkCommandBuffer commandBuffer;
    };
    struct WindowSize
    {
        uint32_t width;
        uint32_t height;
    };
    class Renderer
    {
        public:
            Renderer(std::function<void(VkInstance,VkSurfaceKHR*)> bruh);
            ~Renderer();
            void Render(math::Transform3D& camerapos, float camerafov);
            VkSurfaceKHR surface;

        private:
            void InitRenderer(std::function<void(VkInstance,VkSurfaceKHR*)> bruh);
            void CreateSwapchain();
            void CreatePipeline();
            VkShaderModule LoadShader(std::string name);

            VkDevice device;
            VkPhysicalDevice physicalDevice;
            VkInstance instance;
            VkQueue queue;
            uint32_t queueFamily;
            VmaAllocator allocator;

            VkSwapchainKHR swapchain;
            std::vector<VkImage> swapImages;
            std::vector<VkImageView> swapImageViews;
            VkFormat swapImageFormat;
            std::vector<VkSemaphore> renderSemaphores;
            bool updateSwapchain = false;
            WindowSize window;

            VkImage depthImage;
            VkImageView depthImageView;
            VmaAllocation depthImageAllocation;
            VkFormat depthFormat;

            VkCommandPool commandPool;
            VkPipelineLayout pipelineLayout;
            VkPipeline pipeline;



            framedata frames[2];
            uint8_t currentFrame = 0;

            static inline void chk(VkResult result) {
                if (result != VK_SUCCESS) {
                    std::cerr << "Vulkan call returned an error (" << result << ")\n";
                    exit(result);
                }
            }

            inline void chkSwapchain(VkResult result) {
                if (result < VK_SUCCESS) {
                    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                        updateSwapchain = true;
                        return;
                    }
                    std::cerr << "Vulkan call returned an error (" << result << ")\n";
                    exit(result);
                }
            }
    };
}