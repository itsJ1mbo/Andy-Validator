#include "AndyValidator_Window/Window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "dearImgui/imgui.h"
#include "dearImgui/backends/imgui_impl_glfw.h"
#include "dearImgui/backends/imgui_impl_opengl2.h"
#include <iostream>


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
    if (!initGlfw()) {
        std::cout << "Error inicializando GLFW\n";
        return false;
    }

    if (!initImgui()) {
        std::cout << "Error inicializando dearImgui\n";
        return false;
    }

    return true;
}

bool Window::shouldWindowClose() const
{
    return glfwWindowShouldClose(_glfwWindow);
}

void Window::updateWindow() const
{
    //eventos ventana
    glfwPollEvents();

    processInput();

    //cargar modelo o algo yo que se no se como se hace eso


    //render
    render();
}
 
void Window::processInput() const
{
    if (glfwGetKey(_glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(_glfwWindow, true);
}

void Window::render() const
{

    //llamar al render del modelo o algo no se como va esto

    //renderizamos las ventanas de imgui
    renderImgui();

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
        std::cout << "La ventana se ha cagado encima\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(_glfwWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.IniFilename = nullptr; //porque nos la pela la persistencia de la ventana

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();    

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(_glfwWindow, true);

    ImGui_ImplOpenGL2_Init();

    return true;
}

void Window::renderImgui() const
{

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //pruebita 
    static bool abierto = true;
    panelCustomPrueba(&abierto);

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

void Window::panelCustomPrueba(bool* open) const
{
    //lo que estoy siguiendo es ShowExampleAppLayout de imgui_demo.h

    //primero le damos tamaño, usamos el mainviewport (en vez de _width y _height) 
    //junto con la flag de always para que este escalado se aplique siempre y no solo una vez al principio
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x/2, ImGui::GetMainViewport()->Size.y), ImGuiCond_Always);

    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoDecoration;

    //el booleano controla si la ventana está abierta o no, con ponerlo a false se cierra sola la ventana y tal
    if (ImGui::Begin("Prueba", open, flags))
    {

        //ehhhh no se muy bien que hace todo este if, con o sin el todo esta igual
        /*if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Close", "Ctrl+W")) { *open = false; }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }*/

        // todo esto hace un subpanel con una lista de cosas
        static int selected = 0;
        {
            ImGui::BeginChild("left pane", ImVec2(150, 0), ImGuiChildFlags_Border);
            for (int i = 0; i < 10 ; i++)
            {
                // FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
                char label[128];
                sprintf_s(label, "modelo%d.fbx", i);
                if (ImGui::Selectable(label, selected == i))
                    selected = i;
            }
            ImGui::EndChild();
        }
        //huh? no se que hace
        ImGui::SameLine();
    }

    //SIEMPRE hay que llamar a un end por cada begin
    ImGui::End(); 
}
