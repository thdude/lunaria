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
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

        window = glfwCreateWindow(800, 600, Engine::gameName, nullptr, nullptr);

        //lunaria::Renderer renderer{};
        renderer = new Renderer();

        loop();
    }

    void Window::loop()
    {
        //while(!glfwWindowShouldClose(window))
        while(true)
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