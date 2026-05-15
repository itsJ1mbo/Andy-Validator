#include "AndyValidator_Window/Window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "dearImgui/imgui.h"
#include "dearImgui/backends/imgui_impl_glfw.h"
#include "dearImgui/backends/imgui_impl_opengl2.h"
#include "magicEnum/magic_enum.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <filesystem>
#include <utility>

const char* vertexShaderSource = "#version 120\n"
"attribute vec3 aPos;\n"
"uniform mat4 matrix;\n"
"void main()\n"
"{\n"
"    gl_Position = matrix * (vec4(aPos, 1.0));\n"
"}\n";

const char* fragmentShaderSource = "#version 120\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n" // gl_FragColor es el color con el que se pintan los objetos
"}\n";


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Window::Window(int width, int height) : _width(width), _height(height), _glfwWindow(nullptr), _collapsed(true), _bufferCount(0), _vao(0), _vbo(0), _ebo(0), _shaderProgram(0)
{

}

Window& Window::instance()
{
    if(!_instance)
    {
        _instance = std::unique_ptr<Window>(new Window());
    }

    return *_instance;
}

bool Window::init()
{
    if(!_instance)
    {
        return false;
    }

    return initWindow();
}

void Window::free() const
{
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
    glDeleteProgram(_shaderProgram);
    glfwDestroyWindow(_glfwWindow);
}

bool Window::initWindow()
{
    if(!initGlfw()) {
#if _DEBUG
        std::cout << "Error inicializando GLFW\n";
#endif
        return false;
    }

    if(!initOpenGL()) {
#if _DEBUG
        std::cout << "Error inicializando OpenGL\n";
#endif
        return false;
    }

    if(!initImgui()) {
#if _DEBUG
        std::cout << "Error inicializando dearImgui\n";
#endif
        return false;
    }

    return true;
}

bool Window::shouldWindowClose() const
{
    return glfwWindowShouldClose(_glfwWindow);
}

void Window::updateWindow(const std::vector<ModelResults>& results)
{
    //eventos ventana
    glfwPollEvents();

    processInput();

    //render
    render(results);
}

void Window::setModelNames(const std::vector<std::string>& paths)
{
    for(const auto& path : paths) {
        std::filesystem::path p(path);
        _modelNames.push_back(p.filename().string());
    }
}

void Window::processInput() const
{
    if(glfwGetKey(_glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(_glfwWindow, true);
}

void Window::render(const std::vector<ModelResults>& results)
{
    //limpiamos la pantalla esto es como volver a fp1
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //renderizar el modelo
    renderModel();

    //renderizamos las ventanas de imgui
    renderImgui(results);

    //el back buffer pasa a ser el front buffer (opengl funciona con dos bufferes de colores, el que esta (front) y al que se le aplican todos los comandos y tal (back))
    glfwSwapBuffers(_glfwWindow);
}

bool Window::initGlfw()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //Crear ventana
    _glfwWindow = glfwCreateWindow(_width, _height, "AndyValidator", NULL, NULL);

    if(_glfwWindow == NULL)
    {
#if _DEBUG
        std::cout << "La ventana se ha cagado encima\n";
#endif
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(_glfwWindow);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
#if _DEBUG
        std::cout << "Failed to initialize GLAD\n";
#endif
        return false;
    }

    glViewport(0, 0, _width, _height);
    glfwSetFramebufferSizeCallback(_glfwWindow, framebuffer_size_callback);

    return true;
}

bool Window::initOpenGL()
{
    //pillamos los shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    //los compilamos
    int success;
    char infoLog[512];
    //primero el de vertices
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    //lo mismo pero con el de fragmentos
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }

    //los linkamos al shader program
    _shaderProgram = glCreateProgram();
    glAttachShader(_shaderProgram, vertexShader);
    glAttachShader(_shaderProgram, fragmentShader);
    glLinkProgram(_shaderProgram);

    //comprobamos que estan linkados
    glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }

    //los borramos porque ya estan en el shader program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //depth test para que no se solapen mal
    glEnable(GL_DEPTH_TEST);

    //importante generar los buffers que usara opengl
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    return true;
}

bool Window::initImgui() const
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    if (ImGui::CreateContext() == nullptr)
        return false;
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.IniFilename = nullptr; //porque nos la pela la persistencia de la ventana

    io.Fonts->AddFontDefault();
    io.FontGlobalScale = 1.0;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();    

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(_glfwWindow, true);

    ImGui_ImplOpenGL2_Init();

    return true;
}

void Window::renderImgui(const std::vector<ModelResults>& results)
{

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //pruebita 
    //static bool abierto = true;
    //panelCustomPrueba(&abierto);
    //ImGui::ShowDemoWindow();

    createPanel(results);

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(_glfwWindow, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
    // you may need to backup/reset/restore other state, e.g. for current shader using the commented lines below.
    //GLint last_program;
    //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    //glUseProgram(0);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    //glUseProgram(last_program);
}

void Window::renderModel()
{
    //nos saltamos renderizar esto si no hay nada en el buffer
    if (_bufferCount == 0)
        return;

    //le decimos que use los shaders que hemos cargado antes
    glUseProgram(_shaderProgram);

    //pillamos del shader el parametro
    GLint transformLoc = glGetUniformLocation(_shaderProgram, "matrix");

    //gracias glm por ahorrarme tener que hacer estas cosas a mano
    float fov = 45.0;
    float nearPlane = 0.1;
    float farPlane = 1000;
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)_width / (float)_height, nearPlane, farPlane);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

    //para cambiarle la escala a la que se ve todo cambiar el float de distancia
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(1.25, 0, 0));

    glm::mat4 mvp = projection * view * model;

    //mandar cosas a gpu
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mvp));
    glBindVertexArray(_vao);

    //pum dibuja
    glDrawElements(GL_TRIANGLES, _bufferCount, GL_UNSIGNED_INT, 0);

    //reseteamos las cosas para que imgui no implosione
    glUseProgram(0);
    glBindVertexArray(0);
}
void Window::setModelToBuffers(const ModelResults& result)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    //esto es porque un fbx puede tener varias meshes y los indices empiezan para cada una en el 0,
    //enotnces necesitamos esto para evitar solapamientos raros
    unsigned int vertexOffset = 0;

    //pillamos vertices e indices
    for(const auto& mesh : result.model.meshes)
    {
        for(const auto& v : mesh.vertexes)
        {
            vertices.push_back(v);
        }

        for(int idx : mesh.indexes)
        {
            indices.push_back(idx + vertexOffset);
        }

        vertexOffset += (unsigned int)mesh.vertexes.size();
    }

    //esto es cuanto tiene que leer opengl del buffer, el tamano cambiara dependiendo de lo que se le meta al buffer,
    //pero es NECESARIO actualizarlo a lo que tenga el modelo porque si no no va a dibujar todo
    _bufferCount = (GLsizei)indices.size();

    //bindear buffers
    glBindVertexArray(_vao);

    //vbo: Datos de los vértices
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    //ebo: Datos de los índices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    /**
     * Como se debe interpretar vertex data. Parametros en orden:
     * - Como en el vertex shader el parametro que queremos pasar esta en aPos lo pasamos aqui
     * - Num valores que tiene el vertice (nosotros tenemos 3 floats porque solo guardamos posicion pero podriamos tener mas cosas)
     * - Tipo de los datos a pasar
     * - Si queremos normalizar los datos
     * - Tamano entre cada vertice. En este caso sabemos que el array de los vertices esta compacto perfectamente (no hay huecos en memoria entre vertices)
     * por lo que como cada vertice tiene 3 floats la distancia entre un vertice y otro es 3*sizeof(float). En caso de que un array este perfectamente compacto
     * se puede dejar que sea OpenGL quien defina esta distancia poniendole el valor 0
     * - Offset de donde esta la primera posicion de datos en el buffer (en este caso 0 al estar al principio del array)
     **/
    GLint posAttrib = glGetAttribLocation(_shaderProgram, "aPos");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(posAttrib);

    // unbindeamos para que imgui no se rompa
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void Window::createPanel(const std::vector<ModelResults>& results)
{
    //primero le damos tamaño, usamos el mainviewport (en vez de _width y _height) 
    //junto con la flag de always para que este escalado se aplique siempre y no solo una vez al principio
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x / 2, ImGui::GetMainViewport()->Size.y), ImGuiCond_Always);

    //lo mandamos a la esquina superior izquierda
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    //no queremos que tenga menus ni titulo ni nada
    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoDecoration;

    //el booleano controla si la ventana está abierta o no, con ponerlo a false se cierra sola la ventana y tal, 
    //si quisieramos tener control sobre esto almacenariamos el booleano en algun lado, pero nunca la vamos a querer cerrar
    bool open = true;
    if(ImGui::Begin("Desplegables", &open, flags))
    {
        //muy feo pero necesario para los botones de colapsar/desplegar, ya que si no no podemos desplegarlos
        bool buttonPressed = false;

        if(ImGui::Button("Desplegar todos"))
        {
            _collapsed = false;
            buttonPressed = true;
        }

        ImGui::SameLine();

        if(ImGui::Button("Colapsar todos"))
        {
            _collapsed = true;
            buttonPressed = true;
        }

        for(int i = 0; i < results.size(); i++) {
            createResultDropdown(results[i], i, buttonPressed);
        }
    }
    //SIEMPRE hay que llamar a un end por cada begin
    ImGui::End();
}

void Window::createResultDropdown(const ModelResults& result, int index, bool buttonPressed)
{
    //podemos pasarle la direccion de memoria del resultado a imgui como id para que cada dropdown sea independiente de los demas
    ImGui::PushID(&result);

    if(buttonPressed)
        ImGui::SetNextItemOpen(!_collapsed);
    //quizas un poco feo tener esto asi, pero imgui solo dibuja los contenidos si el header esta abierto,
    //y como queremos tener un hover por si el nombre del archivo es demasiado largo, tenemos que hacerlo asi
    //(si estuviera dentro de la condicion de abierto solo se ejecutaria cuando el header estuviera abierto)
    bool abierto = ImGui::CollapsingHeader(_modelNames[index].c_str());

    if(ImGui::IsItemHovered())
        ImGui::SetTooltip(_modelNames[index].c_str());

    ImGui::SameLine();
    if(!result.index.has_value())
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "(Pendiente)");
    else
        ImGui::TextColored(result.allTestsPassed ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1), result.allTestsPassed ? "(Aprobado)" : "(Suspenso)");


    if(abierto)
    {
        if (!result.index.has_value()) {
            ImGui::Text("Pendiente por validar.");
        }
        else {
            for (ValidationResult val : result.validations)
            {
                ImGui::BulletText("Resultado de");
                ImGui::SameLine();
                //libreria magica que convierte de enum a string
                ImGui::Text(magic_enum::enum_name(val.type).data());

                //tooltip de descripcion del test
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip(val.description.c_str());

                ImGui::SameLine();
                ImGui::TextColored(val.passed ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1), val.passed ? "Aprobado" : "Suspenso");
            }
            if (ImGui::Button("Display model"))
            {
                //para setear la info de los modelos a los buffers de opengl
                setModelToBuffers(result);
            }
        }
    }
    //importante hacer que imgui deje de usar la id que le hemos dicho que use para el objeto
    ImGui::PopID();
}