#pragma once
#include "ModelData.h"
#include <memory>

struct GLFWwindow;

class Window 
{
public:
    static Window& instance();

    bool init();
    void free();

    bool initWindow();
    //<summary>
    //indica si la ventana deberia cerrarse
    bool shouldWindowClose() const;
    //<summary>
    //actualiza la ventana (input y render)
    void updateWindow(const std::vector<ModelResults>& results);
    //<summary>
    //setea un vector con el nombre de los modelos para no depender del vector de resultados para el render
    void setModelNames(const std::vector<std::string>& paths);

private:
    Window(int width = 1200, int height = 900);
    Window(const Window& in) = delete;
    Window& operator=(const Window& in) = delete;
    Window(const Window&& in) = delete;
    Window& operator=(const Window&& in) = delete;

    //<summary>
    //Crea e inicializa la ventana de GLFW
    bool initGlfw();
    //<summary>
    //Inicializa todo lo relacionado con OpenGL
    bool initOpenGL();
    //<summary>
    //Inicializa todo lo relacionado con ImGui
    bool initImgui() const;

    //<summary>
    //compila y attachea un shader a un shader program
    bool compileAndAttachShader(unsigned int shaderProgram, const char* shaderSource, unsigned int shaderType);

    //<summary>
    //Gestiona input de la ventana
    void processInput() const;
    //<summary>
    //Renderiza toda la ventana
    void render(const std::vector<ModelResults>& results);
    //<summary>
    //Renderiza el modelo seleccionado
    void renderModel();
    //<summary>
    //Renderiza la ventana de imgui
    void renderImgui(const std::vector<ModelResults>& results);

    //<summary>
    //Envia los datos del fbx seleccionado a los buffers de opengl para que el modelo se renderice en el siguiente frame
    void setModelToBuffers(const ModelResults& result);

    //<summary>
    //crea toda la ventana de imgui
    void createPanel(const std::vector<ModelResults>& results);
    //<summary>
    //crea el desplegable con los resultados de cada modelo
    void createResultDropdown(const ModelResults& resultado, int index, bool buttonPressed);
    
    inline static std::unique_ptr<Window> _instance;

    int _width;
    int _height;
    GLFWwindow* _glfwWindow;
    std::vector<std::string> _modelNames;

    //-------- Cosas de OpenGL --------
    //buffers y cosas de opengl
    unsigned int _vao, _vbo, _ebo, _defaultShaderProgram, _visualizeNormalsShaderProgram;

    //indica cuanto tiene que leer del buffer el gldrawarrays
    int _bufferCount;

    //vector con ids texturas actuales
    std::vector<unsigned int> _activeTextures;
    //esto fijo que esta feo y no se hace asi pero no se me ocurre otra forma ahora mismo, pero es basicamente una textura por defecto
    //por si un modelo no tiene texturas que no se vea negro
    unsigned int _fallbackTexture = 0;

    //cosas de la rotacion del modelo
    float _modelRotationAngle;
    float _modelRotationSpeed;

    //cosas para que quepa en la ventana
    glm::vec3 _modelCenter;
    float _modelScaleFactor;
    //---------------------------------

    //-------- Cosas de Imgui --------
    //para los botones de desplegar y colapsar todas las validaciones
    bool _collapsed;
    //para controlar en que ejes rota el modelo
    bool _rotX, _rotY, _rotZ;
    //para controlar si se visualizan las normales
    bool _visualizeNormals;
    //para controlar si ver el wireframe o no
    bool _visualizeWireframe;
    //--------------------------------
};