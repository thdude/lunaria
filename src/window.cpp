#include "window.h"
#include "engine.h"
#include "renderer.h"
#include <iostream>
#include <GLFW/glfw3.h>

namespace lunaria
{
    Window::Window()
    {
        std::cout << "creating a new window" << std::endl;

        if(!glfwInit())
        {
            std::cout << "glfw init failed!!" << std::endl;
        }

        InitWindow();
    }

    Window::~Window()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::InitWindow()
    {

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(800, 600, Engine::gameName, nullptr, nullptr);

        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

        

        renderer = new Renderer([=](VkInstance instance, VkSurfaceKHR *surface) -> void { glfwCreateWindowSurface(instance, window, nullptr, surface); });

        loop();
    }

    void Window::loop()
    {
        while(!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }
    }

    void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height) 
    {
        auto w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        w->framebufferResized = true;
        w->width = width;
        w->height = height;
    }

}