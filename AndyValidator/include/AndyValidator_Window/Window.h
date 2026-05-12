#pragma once
#include "ModelData.h"
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
    void updateWindow(const std::vector<Results>& results) const;
    void setModelNames(const std::vector<std::string>& paths);

private:
    Window(int width = 800, int height = 600);
    Window(const Window& in) = delete;
    Window& operator=(const Window& in) = delete;
    Window(const Window&& in) = delete;
    Window& operator=(const Window&& in) = delete;

    void processInput() const;
    void render(const std::vector<Results>& results) const;
    bool initGlfw();
    bool initImgui() const;
    void renderImgui(const std::vector<Results>& results) const;

    void createPanel(const std::vector<Results>& results) const;

    void createResultDropdown(const Results& resultado, int index) const;
    
    inline static std::unique_ptr<Window> _instance;

    int _width;
    int _height;
    GLFWwindow* _glfwWindow;
    std::vector<std::string> _modelNames;
};