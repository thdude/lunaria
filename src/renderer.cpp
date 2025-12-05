#include "renderer.h"
#include "engine.h"
#include "VkBootstrap.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace lunaria
{
    Renderer::Renderer()
    {
        InitRenderer();
    }
    Renderer::~Renderer()
    {
        //buh
    }

    void Renderer::InitRenderer()
    {
        vkb::InstanceBuilder builder;
        auto inst_ret = builder.set_app_name (Engine::gameName)
                            .set_engine_name("lunaria")
                            .request_validation_layers ()
                            .use_default_debug_messenger ()
                            .build ();
        if (!inst_ret) { /* report */ }
        vkb::Instance vkb_inst = inst_ret.value ();

        vkb::PhysicalDeviceSelector selector{ vkb_inst };
        auto phys_ret = selector.set_surface (surface)
                            .set_minimum_version (1, 1)
                            .require_dedicated_transfer_queue ()
                            .select ();
        if (!phys_ret) { /* report */ }

        vkb::DeviceBuilder device_builder{ phys_ret.value () };
        auto dev_ret = device_builder.build ();
        if (!dev_ret) { /* report */ }
        vkb::Device vkb_device = dev_ret.value ();

        auto graphics_queue_ret = vkb_device.get_queue (vkb::QueueType::graphics);
        if (!graphics_queue_ret)  { /* report */ }
        VkQueue graphics_queue = graphics_queue_ret.value ();
    }

    void Renderer::Render()
    {

    }
}