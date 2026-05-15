#pragma once
#include "ModelData.h"
#include <memory>

struct GLFWwindow;

class Window 
{
public:
    static Window& instance();

    bool init();
    void free() const;

    bool initWindow();
    bool shouldWindowClose() const;
    void updateWindow(const std::vector<ModelResults>& results);
    void setModelNames(const std::vector<std::string>& paths);

private:
    Window(int width = 1200, int height = 900);
    Window(const Window& in) = delete;
    Window& operator=(const Window& in) = delete;
    Window(const Window&& in) = delete;
    Window& operator=(const Window&& in) = delete;

    void processInput() const;
    void render(const std::vector<ModelResults>& results);
    bool initGlfw();
    bool initImgui() const;
    void renderImgui(const std::vector<ModelResults>& results);

    void createPanel(const std::vector<ModelResults>& results);

    void createResultDropdown(const ModelResults& resultado, int index, bool buttonPressed);
    
    inline static std::unique_ptr<Window> _instance;

    int _width;
    int _height;
    GLFWwindow* _glfwWindow;
    std::vector<std::string> _modelNames;

    bool _collapsed;
    int selectedIndex;
};