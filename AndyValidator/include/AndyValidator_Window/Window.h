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
    bool initOpenGL();
    bool initImgui() const;
    void renderImgui(const std::vector<ModelResults>& results);
    void renderModel();
    void setModelToBuffers(const ModelResults& result);

    void createPanel(const std::vector<ModelResults>& results);

    void createResultDropdown(const ModelResults& resultado, int index, bool buttonPressed);
    
    inline static std::unique_ptr<Window> _instance;

    int _width;
    int _height;
    GLFWwindow* _glfwWindow;
    std::vector<std::string> _modelNames;

    //buffers y cosas de opengl
    unsigned int _vao, _vbo, _ebo, _shaderProgram;

    //cosas de la rotacion del modelo
    float _modelRotationAngle;
    float _modelRotationSpeed;

    //cosas para que quepa en la ventana
    glm::vec3 _modelCenter;
    float _modelScaleFactor;

    //indica cuanto tiene que leer del buffer el gldrawarrays
    int _bufferCount;

    bool _collapsed;
};