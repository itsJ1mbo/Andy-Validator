#include "AndyValidator_Window/Window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "dearImgui/imgui.h"
#include "dearImgui/backends/imgui_impl_glfw.h"
#include "dearImgui/backends/imgui_impl_opengl2.h"
#include "magicEnum/magic_enum.hpp"
#include <iostream>
#include <filesystem>
#include <utility>


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Window::Window(int width, int height) : _width(width), _height(height), _glfwWindow(nullptr)
{

}

Window& Window::instance() 
{
    if (!_instance)
    {
        _instance = std::unique_ptr<Window>(new Window());
    }

    return *_instance;
}

bool Window::init()
{
    if (!_instance) 
    {
        return false;
    }
    
	return initWindow();
}

void Window::free() const
{
    glfwDestroyWindow(_glfwWindow);
}

bool Window::initWindow()
{
    if (!initGlfw()) {
#if _DEBUG
        std::cout << "Error inicializando GLFW\n";
#endif
        return false;
    }

    if (!initImgui()) {
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

void Window::updateWindow(const std::vector<ModelResults>& results) const
{
    //eventos ventana
    glfwPollEvents();

    processInput();

    //cargar modelo o algo yo que se no se como se hace eso


    //render
    render(results);
}

void Window::setModelNames(const std::vector<std::string>& paths)
{
    for (const auto& path : paths) {
        std::filesystem::path p(path);
        _modelNames.push_back(p.filename().string());
    }
}
 
void Window::processInput() const
{
    if (glfwGetKey(_glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(_glfwWindow, true);
}

void Window::render(const std::vector<ModelResults>& results) const
{

    //llamar al render del modelo o algo no se como va esto

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

    if (_glfwWindow == NULL)
    {
#if _DEBUG
        std::cout << "La ventana se ha cagado encima\n";
#endif
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(_glfwWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
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

void Window::renderImgui(const std::vector<ModelResults>& results) const
{

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
    // you may need to backup/reset/restore other state, e.g. for current shader using the commented lines below.
    //GLint last_program;
    //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    //glUseProgram(0);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    //glUseProgram(last_program);
}

void Window::createPanel(const std::vector<ModelResults>& results) const
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
    if (ImGui::Begin("Desplegables", &open, flags))
    {
        ImGui::TextColored(ImVec4(1, 0, 1, 1), "Titulo muy guapo que es completamente innecesario:");
        for (int i = 0; i < results.size(); i++) {
            createResultDropdown(results[i], i);
        }
    }
    //SIEMPRE hay que llamar a un end por cada begin
    ImGui::End();
}

void Window::createResultDropdown(const ModelResults& result, int index) const
{
    //podemos pasarle la direccion de memoria del resultado a imgui como id para que cada dropdown sea independiente de los demas
    ImGui::PushID(&result);

    //quizas un poco feo tener esto asi, pero imgui solo dibuja los contenidos si el header esta abierto,
    //y como queremos tener un hover por si el nombre del archivo es demasiado largo, tenemos que hacerlo asi
    //(si estuviera dentro de la condicion de abierto solo se ejecutaria cuando el header estuviera abierto)
    bool abierto = ImGui::CollapsingHeader(_modelNames[index].c_str());

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip(_modelNames[index].c_str());

    ImGui::SameLine();
    if (!result.index.has_value())
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "(Pendiente)");
    else
        ImGui::TextColored(result.allTestsPassed ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1), result.allTestsPassed ? "(Aprobado)" : "(Suspenso)");


    if (abierto)
    {
        if (!result.index.has_value()) {
            ImGui::Text("Pendiente por validar.");
        }
        else {
            for(ValidationResult val : result.validations)
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
        }
    }
    //importante hacer que imgui deje de usar la id que le hemos dicho que use para el objeto
    ImGui::PopID();
}