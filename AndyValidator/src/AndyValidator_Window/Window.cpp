#include "AndyValidator_Window/Window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Window::Window(int width, int height) : _width(width), _height(height), _glfwWindow(nullptr)
{

}

const Window& Window::instance() 
{
    if (!_instance)
    {
        _instance = std::unique_ptr<Window>(new Window());
    }

    return *_instance;
}

bool Window::init() const
{
    if (_instance) 
    {
        return _instance->initWindow();
    }
    else
    {
        return false;
    }
}

void Window::free() const
{
    glfwDestroyWindow(_glfwWindow);
}

bool Window::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //Crear ventana
    _glfwWindow = glfwCreateWindow(_width, _height, "AndyValidator", NULL, NULL);

    if (_glfwWindow == NULL)
    {
        std::cout << "La ventana se ha cagado encima";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(_glfwWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glViewport(0, 0, _width, _height);
    glfwSetFramebufferSizeCallback(_glfwWindow, framebuffer_size_callback);

    return true;
}

bool Window::shouldWindowClose() const
{
    return glfwWindowShouldClose(_glfwWindow);
}

void Window::updateWindow() const
{
    processInput();

    //cargar modelo o algo yo que se
    
    //el back buffer pasa a ser el front buffer (opengl funciona con dos bufferes de colores, el que esta (front) y al que se le aplican todos los comandos y tal (back))
    glfwSwapBuffers(_glfwWindow);
    //eventos ventana
    glfwPollEvents();
}
 
void Window::processInput() const
{
    if (glfwGetKey(_glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(_glfwWindow, true);
}

void Window::render()
{

}
