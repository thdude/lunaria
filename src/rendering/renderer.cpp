#include "renderer.h"
#include "engine.h"
#include "utils/math.h"
#include <volk.h>
#include "VkBootstrap.h"
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <array>
#include <string>
#include <iostream>
#include <fstream>

namespace lunaria
{
    struct ShaderData {
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
    };

    Renderer::Renderer(std::function<void(VkInstance,VkSurfaceKHR*)> bruh)
    {
        InitRenderer(bruh);
    }
    Renderer::~Renderer()
    {
        //buh
    }
    

    void Renderer::InitRenderer(std::function<void(VkInstance,VkSurfaceKHR*)> bruh)
    {
        volkInitialize();

        vkb::InstanceBuilder builder;
        auto inst_ret = builder.set_app_name (Engine::gameName.data())
                            .set_engine_name("lunaria")
                            .request_validation_layers ()
                            .use_default_debug_messenger ()
                            .require_api_version(1, 3)
                            .build ();
        if (!inst_ret) { /* report */ }
        vkb::Instance vkb_inst = inst_ret.value ();

        instance = vkb_inst.instance;
        volkLoadInstanceOnly(instance);
        bruh(vkb_inst, &surface); 

        VkPhysicalDeviceVulkan12Features enabledVk12Features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .descriptorIndexing = true, .shaderSampledImageArrayNonUniformIndexing = true, .descriptorBindingVariableDescriptorCount = true, .runtimeDescriptorArray = true, .bufferDeviceAddress = true };
        VkPhysicalDeviceVulkan13Features enabledVk13Features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &enabledVk12Features, .synchronization2 = true, .dynamicRendering = true };
        VkPhysicalDeviceFeatures enabledVk10Features{ .samplerAnisotropy = VK_TRUE };

        vkb::PhysicalDeviceSelector selector{ vkb_inst };
        auto phys_ret = selector.set_surface (surface)
                            .set_minimum_version (1, 1)
                            .require_dedicated_transfer_queue ()
                            .set_required_features(enabledVk10Features)
                            .set_required_features_13(enabledVk13Features)
                            .select ();
        if (!phys_ret) { /* report */ }
        physicalDevice = phys_ret.value().physical_device;

        vkb::DeviceBuilder device_builder{ phys_ret.value () };
        auto dev_ret = device_builder.build ();
        if (!dev_ret) { /* report */ }
        vkb::Device vkb_device = dev_ret.value ();
        device = vkb_device.device;

        volkLoadDevice(device);

        auto graphics_queue_ret = vkb_device.get_queue (vkb::QueueType::graphics);
        if (!graphics_queue_ret)  { /* report */ }
        queue = graphics_queue_ret.value ();
        auto graphics_queue_family_ret = vkb_device.get_queue_index(vkb::QueueType::graphics);
        queueFamily = graphics_queue_family_ret.value();

        VmaVulkanFunctions vkFunctions{
            .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
            .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
            .vkCreateImage = vkCreateImage
        };
        VmaAllocatorCreateInfo allocatorCI{
            .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT, 
            .physicalDevice = physicalDevice,
            .device = device,
            .pVulkanFunctions = &vkFunctions,
            .instance = instance
        };
        chk(vmaCreateAllocator(&allocatorCI, &allocator));

        CreateSwapchain();

        VkSemaphoreCreateInfo semaphoreCI{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };
        VkFenceCreateInfo fenceCI{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
        };

        for (auto i = 0; i < 2; i++) {
            VkBufferCreateInfo uBufferCI{
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .size = sizeof(ShaderData),
                .usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
            };
            VmaAllocationCreateInfo uBufferAllocCI{
                .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
                .usage = VMA_MEMORY_USAGE_AUTO
            };
            chk(vmaCreateBuffer(allocator, &uBufferCI, &uBufferAllocCI, &frames[i].shaderData, &frames[i].shaderDataAllocation, nullptr));

                VkBufferDeviceAddressInfo uBufferBdaInfo{
                .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
                .buffer = frames[i].shaderData
            };
            frames[i].shaderDataAddress = vkGetBufferDeviceAddress(device, &uBufferBdaInfo);

            chk(vkCreateFence(device, &fenceCI, nullptr, &frames[i].fence));
            chk(vkCreateSemaphore(device, &semaphoreCI, nullptr, &frames[i].presentSemaphore));

            VkCommandPoolCreateInfo commandPoolCI{
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                .queueFamilyIndex = queueFamily
            };
            chk(vkCreateCommandPool(device, &commandPoolCI, nullptr, &commandPool));

            VkCommandBufferAllocateInfo cbAllocCI{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .commandPool = commandPool,
                .commandBufferCount = 1
            };
            chk(vkAllocateCommandBuffers(device, &cbAllocCI, &frames[i].commandBuffer));
        }

        renderSemaphores.resize(swapImages.size());
        for (auto& semaphore : renderSemaphores) {
            chk(vkCreateSemaphore(device, &semaphoreCI, nullptr, &semaphore));
        }

        CreatePipeline();
    }

    void Renderer::CreateSwapchain()
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
        uint32_t width = surfaceCapabilities.currentExtent.width;
        uint32_t height = surfaceCapabilities.currentExtent.height;
        window.width = width;
        window.height = height;

        vkb::SwapchainBuilder swapBuilder{physicalDevice, device, surface};

        swapImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

        vkb::Swapchain vkbSwapchain = swapBuilder
                .set_desired_format(VkSurfaceFormatKHR{.format = swapImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                .set_desired_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR)
                .set_desired_extent(window.width, window.height)
                .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                .build().value();

        swapchain = vkbSwapchain.swapchain;
        swapImages = vkbSwapchain.get_images().value();
        swapImageViews = vkbSwapchain.get_image_views().value();



        //create depth buffer
        std::vector<VkFormat> depthFormatList{ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
        for (VkFormat& format : depthFormatList) {
            VkFormatProperties2 formatProperties{ .sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2 };
            vkGetPhysicalDeviceFormatProperties2(physicalDevice, format, &formatProperties);
            if (formatProperties.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                depthFormat = format;
                break;
            }
        }

        VkImageCreateInfo depthImageCI{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = depthFormat,
            .extent{.width = window.width, .height = window.height, .depth = 1 },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        VmaAllocationCreateInfo allocCI{
            .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };
        chk(vmaCreateImage(allocator, &depthImageCI, &allocCI, &depthImage, &depthImageAllocation, nullptr));

        VkImageViewCreateInfo depthViewCI{ 
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = depthImage,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = depthFormat,
            .subresourceRange{ .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT, .levelCount = 1, .layerCount = 1 }
        };
        chk(vkCreateImageView(device, &depthViewCI, nullptr, &depthImageView));
    }

    void Renderer::CreatePipeline()
    {
        //test for now
        VkShaderModule vertexShader = LoadShader("../shaders/render.vert.spv");
        VkShaderModule fragmentShader = LoadShader("../shaders/render.frag.spv");

        VkPushConstantRange pushConstantRange{
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .size = sizeof(PushConstants)
        };
        VkPipelineLayoutCreateInfo pipelineLayoutCI{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr, //for texture
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &pushConstantRange
        };
        chk(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelineLayout));

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages{
            { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertexShader, .pName = "main"},
            { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragmentShader, .pName = "main" }
        };

        VkPipelineVertexInputStateCreateInfo vertexInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

        VkPipelineInputAssemblyStateCreateInfo inputAssembly
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
        };

        VkPipelineViewportStateCreateInfo viewportState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .scissorCount = 1
        };
        std::vector<VkDynamicState> dynamicStates{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = 2,
            .pDynamicStates = dynamicStates.data()
        };
        VkPipelineDepthStencilStateCreateInfo depthStencilState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL
        };
        VkPipelineRenderingCreateInfo renderingCI{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &swapImageFormat,
            .depthAttachmentFormat = depthFormat
        };
        VkPipelineColorBlendAttachmentState blendAttachment{
            .colorWriteMask = 0xF
        };
        VkPipelineColorBlendStateCreateInfo colorBlendState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .attachmentCount = 1,
            .pAttachments = &blendAttachment
        };
        VkPipelineRasterizationStateCreateInfo rasterizationState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .lineWidth = 1.0f
        };
        VkPipelineMultisampleStateCreateInfo multisampleState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT
        };

        VkGraphicsPipelineCreateInfo pipelineCI{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &renderingCI,
            .stageCount = 2,
            .pStages = shaderStages.data(),
            .pVertexInputState = &vertexInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizationState,
            .pMultisampleState = &multisampleState,
            .pDepthStencilState = &depthStencilState,
            .pColorBlendState = &colorBlendState,
            .pDynamicState = &dynamicState,
            .layout = pipelineLayout
        };
        chk(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCI, nullptr, &pipeline));
    }

    VkShaderModule Renderer::LoadShader(std::string name)
    {
        std::ifstream file(name, std::ios::binary);
        std::istreambuf_iterator fileIterator(file);
        std::vector<uint8_t> fileData(fileIterator,{});

        VkShaderModuleCreateInfo shaderModuleCI{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = fileData.size(),
            .pCode = (uint32_t*)fileData.data()
        };
        VkShaderModule shaderModule{};
        chk(vkCreateShaderModule(device, &shaderModuleCI, nullptr, &shaderModule));
        return shaderModule;
    }

    uint32_t Renderer::UploadMesh(Vertex* verts, uint32_t vertCount, uint32_t* indices, uint32_t indexCount)
    {
        Mesh mesh;

        VkBufferCreateInfo uBufferVertexCI{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = sizeof(Vertex) * vertCount,
            .usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
        };
        VmaAllocationCreateInfo uBufferAllocVertexCI{
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };
        chk(vmaCreateBuffer(allocator, &uBufferVertexCI, &uBufferAllocVertexCI, &mesh.vertexBuffer, &mesh.vertexAllocation, nullptr));

            VkBufferDeviceAddressInfo uBufferBdaInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
            .buffer = mesh.vertexBuffer
        };
        mesh.address = vkGetBufferDeviceAddress(device, &uBufferBdaInfo);
        VkBufferCreateInfo uBufferIndexCI{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = sizeof(uint32_t) * indexCount,
            .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT
        };
        VmaAllocationCreateInfo uBufferAllocIndexCI{
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };
        chk(vmaCreateBuffer(allocator, &uBufferIndexCI, &uBufferAllocIndexCI, &mesh.indexBuffer, &mesh.indexAllocation, nullptr));

        mesh.vertCount = vertCount;
        mesh.indexCount = indexCount;

        void* mappedData = mesh.vertexAllocation->GetMappedData();
        memcpy(mappedData, verts, sizeof(Vertex) * vertCount);
        mappedData = mesh.indexAllocation->GetMappedData();
        memcpy(mappedData, indices, sizeof(uint32_t) * indexCount);

        meshes.push_back(mesh);
        return meshes.size() - 1;
    }

    void Renderer::Render(math::Transform3D& camerapos, float camerafov)
    {
        framedata& frame = frames[currentFrame];
        uint32_t imageIndex;

        

        ShaderData shaderdata;
        shaderdata.projection = glm::perspective(glm::radians(camerafov), (float)window.width / window.height, 0.1f, 1000.0f);
        shaderdata.projection[1][1] *= -1;
        shaderdata.view = camerapos.GetViewMatrix();
        shaderdata.model = {1};

        void* mappedData = frame.shaderDataAllocation->GetMappedData();
        memcpy(mappedData, &shaderdata, sizeof(ShaderData));

        chk(vkWaitForFences(device, 1, &frame.fence, true, UINT64_MAX));
        chk(vkResetFences(device, 1,  &frame.fence));

        chkSwapchain(vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, frame.presentSemaphore, VK_NULL_HANDLE, &imageIndex));

        auto cb = frame.commandBuffer;
        chk(vkResetCommandBuffer(cb, 0));

        VkCommandBufferBeginInfo cbBI {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        };

        ////////////////////////////////
        //START COMMAND BUFFER
        ////////////////////////////////

        chk(vkBeginCommandBuffer(cb, &cbBI));

        std::array<VkImageMemoryBarrier2, 2> outputBarriers{
            VkImageMemoryBarrier2{
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = 0,
                .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                .image = swapImages[imageIndex],
                .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1 }
            },
            VkImageMemoryBarrier2{
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
                .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                .image = depthImage,
                .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, .levelCount = 1, .layerCount = 1 }
            }
        };
        VkDependencyInfo barrierDependencyInfo{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 2,
            .pImageMemoryBarriers = outputBarriers.data()
        };
        vkCmdPipelineBarrier2(cb, &barrierDependencyInfo);

        VkRenderingAttachmentInfo colorAttachmentInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = swapImageViews[imageIndex],
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue{.color{ 1.0f, 0.0f, 0.0f, 1.0f }}
        };
        VkRenderingAttachmentInfo depthAttachmentInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = depthImageView,
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .clearValue = {.depthStencil = {1.0f,  0}}
        };
        VkRenderingInfo renderingInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea{.extent{.width = window.width, .height = window.height }},
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentInfo,
            .pDepthAttachment = &depthAttachmentInfo
        };
        vkCmdBeginRendering(cb, &renderingInfo);
        VkViewport vp{
            .width = static_cast<float>(window.width),
            .height = static_cast<float>(window.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };
        vkCmdSetViewport(cb, 0, 1, &vp);
        VkRect2D scissor{ .extent{ .width = window.width, .height = window.height } };
        vkCmdSetScissor(cb, 0, 1, &scissor);

        vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        //make loop
        vkCmdBindIndexBuffer(cb, meshes[0].indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        PushConstants pushConstants {frame.shaderDataAddress, meshes[0].address};
        vkCmdPushConstants(cb, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &pushConstants);
        vkCmdDrawIndexed(cb, meshes[0].indexCount, 1, 0, 0, 0);

        
        vkCmdEndRendering(cb);
        VkImageMemoryBarrier2 barrierPresent{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = 0,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .image = swapImages[imageIndex],
            .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1 }
        };
        VkDependencyInfo barrierPresentDependencyInfo{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &barrierPresent
        };
        vkCmdPipelineBarrier2(cb, &barrierPresentDependencyInfo);
        vkEndCommandBuffer(cb);
        ////////////////////////////////
        //END COMMAND BUFFER
        ////////////////////////////////

        VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &frame.presentSemaphore,
            .pWaitDstStageMask = &waitStages,
            .commandBufferCount = 1,
            .pCommandBuffers = &cb,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &renderSemaphores[imageIndex],
        };
        chk(vkQueueSubmit(queue, 1, &submitInfo, frame.fence));
        currentFrame = (currentFrame + 1) % 2;

        VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &renderSemaphores[imageIndex],
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &imageIndex
        };
        chkSwapchain(vkQueuePresentKHR(queue, &presentInfo));
    }
}