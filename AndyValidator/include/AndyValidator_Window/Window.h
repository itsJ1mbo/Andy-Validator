#pragma once

#include <memory>

struct GLFWwindow;

class Window 
{
public:
    static Window& instance();

    bool init() const;
    void free() const;

    bool initWindow();
    bool shouldWindowClose() const;
    void updateWindow() const;

private:
    Window(int width = 800, int height = 600);
    Window(const Window& in) = delete;
    Window& operator=(const Window& in) = delete;
    Window(const Window&& in) = delete;
    Window& operator=(const Window&& in) = delete;

    void processInput() const;
    void render() const;
    bool initGlfw();
    bool initImgui() const;
    void renderImgui() const;

    void panelCustomPrueba(bool* open) const;
    
    inline static std::unique_ptr<Window> _instance;

    int _width;
    int _height;
    GLFWwindow* _glfwWindow;
};