#pragma once

class GLFWwindow;

class Window {
public:
    Window(int width = 800, int height = 600);
    ~Window();

    bool initWindow();

    bool shouldWindowClose();

    void updateWindow();
private:

    void processInput();

    void Render();

    int _width;
    int _height;
    GLFWwindow* _glfwWindow;
};