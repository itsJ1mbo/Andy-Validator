#include "AndyValidator_Window/Window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "dearImgui/imgui.h"
#include "dearImgui/backends/imgui_impl_glfw.h"
#include "dearImgui/backends/imgui_impl_opengl2.h"
#include "magicEnum/magic_enum.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "stb_image.h"
#include "AndyValidator_Window/shaders.h"
#include <iostream>
#include <filesystem>
#include <utility>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Window::Window(int width, int height) : _width(width), _height(height), _glfwWindow(nullptr), _collapsed(true), _bufferCount(0), _vao(0), _vbo(0), _ebo(0), _defaultShaderProgram(0), _visualizeNormalsShaderProgram(0), _modelRotationAngle(0), _modelRotationSpeed(1.0), _rotX(false), _rotY(true), _rotZ(false), _visualizeNormals(false)
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

void Window::free()
{
    if (!_activeTextures.empty()) {
        glDeleteTextures((GLsizei)_activeTextures.size(), _activeTextures.data());
        _activeTextures.clear();
    }
    if (_fallbackTexture != 0) {
        glDeleteTextures(1, &_fallbackTexture);
    }
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
    glDeleteProgram(_defaultShaderProgram);
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

    //procesa input
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
    //ahora mismo solo tenemos que se pueda cerrar la aplicacion pulsando el esc, pero se podria ampliar metiendo controles de camara por ejemplo
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _glfwWindow = glfwCreateWindow(_width, _height, "AndyValidator", NULL, NULL);

    if(_glfwWindow == NULL)
    {
#if _DEBUG
        std::cout << "Fallo al inicializar la ventana\n";
#endif
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(_glfwWindow);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
#if _DEBUG
        std::cout << "Fallo al inicializar GLAD\n";
#endif
        return false;
    }

    glViewport(0, 0, _width, _height);
    glfwSetFramebufferSizeCallback(_glfwWindow, framebuffer_size_callback);

    return true;
}

bool Window::initOpenGL()
{
    int success;
    char infoLog[512];

    //shaders default
    //------------------
    _defaultShaderProgram = glCreateProgram();
    compileAndAttachShader(_defaultShaderProgram, vertexShaderSource, GL_VERTEX_SHADER);
    compileAndAttachShader(_defaultShaderProgram, fragmentShaderSource, GL_FRAGMENT_SHADER);
    glLinkProgram(_defaultShaderProgram);

    //comprobamos que estan linkados
    glGetProgramiv(_defaultShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_defaultShaderProgram, 512, NULL, infoLog);
#if _DEBUG
        std::cout << "ERROR::DEFAULT::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
#endif
        return false;
    }
    //------------------

    //shaders de visualizacion de normales
    //------------------
    _visualizeNormalsShaderProgram = glCreateProgram();
    compileAndAttachShader(_visualizeNormalsShaderProgram, visualizeNormalsVertexShaderSource, GL_VERTEX_SHADER);
    compileAndAttachShader(_visualizeNormalsShaderProgram, visualizeNormalsFragmentShaderSource, GL_FRAGMENT_SHADER);
    glLinkProgram(_visualizeNormalsShaderProgram);

    //comprobamos que estan linkados
    glGetProgramiv(_visualizeNormalsShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_visualizeNormalsShaderProgram, 512, NULL, infoLog);
#if _DEBUG
        std::cout << "ERROR::NORMALS::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
#endif
        return false;
    }
    //------------------


    //depth test para que no se solapen mal los modelos
    glEnable(GL_DEPTH_TEST);

    //le metemos el culling para las normales 
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    //importante generar los buffers que usara opengl
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    //cremaos textura por defecto
    glGenTextures(1, &_fallbackTexture);
    glBindTexture(GL_TEXTURE_2D, _fallbackTexture);

    //seteamos wrappeo y filtrado como en las texturas normales
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //nuestra textura sera un unico pixel gris (no se en blender es gris, digo yo que el gris queda bien)
    unsigned char greyPixel[] = { 200, 200, 200};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, greyPixel);

    //unbindeamos la textura para no romper otras inicializaciones
    glBindTexture(GL_TEXTURE_2D, 0); 

    return true;
}

bool Window::initImgui() const
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    if (ImGui::CreateContext() == nullptr)
        return false;
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    //como nos da igual que se conserven los ajustes de la ventana entre ejecuciones, hacemos que no se genere ningun imgui.ini
    io.IniFilename = nullptr; 

    //fuente de la ventana de imgui, usamos la default para no tener que incluir un .ttf con el .exe
    io.Fonts->AddFontDefault();
    io.FontGlobalScale = 1.0;

    //cambiar el estilo de la ventana a lo que queramos
    ImGuiStyle& style = ImGui::GetStyle();
    //fondo de la ventana
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    //desplegables
    style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
    //botones
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
    //sliders y checkboxes
    style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(_glfwWindow, true);

    ImGui_ImplOpenGL2_Init();

    return true;
}

bool Window::compileAndAttachShader(unsigned int shaderProgram, const char* shaderSource, unsigned int shaderType)
{
    int success;
    char infoLog[512];

    //creamos el shader
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    //compilamos
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
#if _DEBUG
        std::string aux = "";
        shaderType == GL_VERTEX_SHADER ? aux = "VERTEX" : "FRAGMENT";
        std::cout << "ERROR::SHADER::" << aux << "::COMPILATION_FAILED\n" << infoLog << std::endl;
#endif
        return false;
    }

    //attacheamos al shader program
    glAttachShader(shaderProgram, shader);
    //lo borramos porque ya esta attacheado
    glDeleteShader(shader);

    return true;
}

void Window::renderImgui(const std::vector<ModelResults>& results)
{

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //creamos el panel de imgui (si, hay que crearlo en cada frame, imgui funciona asi)
    createPanel(results);

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(_glfwWindow, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void Window::renderModel()
{
    //nos saltamos renderizar esto si no hay nada en el buffer
    if (_bufferCount == 0)
        return;

    //le decimos que use los shaders que hemos cargado antes
    if(_visualizeNormals)
        glUseProgram(_visualizeNormalsShaderProgram);
    else
        glUseProgram(_defaultShaderProgram);

    //si el modelo tiene texturas las ponemos
    if(!_activeTextures.empty())
    {
        for (size_t i = 0; i < _activeTextures.size(); ++i)
        {
            //bindeamos la textura
            glActiveTexture(GL_TEXTURE0 + (GLenum)i);
            glBindTexture(GL_TEXTURE_2D, _activeTextures[i]);

            //sacamos del shader el parametro de textura correspondiente
            std::string uniformName = "texture" + std::to_string(i);
            GLint texLoc = glGetUniformLocation(_defaultShaderProgram, uniformName.c_str());

            //lo asignamos (comprobando primero si el shader que tenemos tiene ese parametro)
            if (texLoc != -1) {
                glUniform1i(texLoc, (GLint)i);
            }
        }
    }
    else
    {
        //si no tiene ponemos la textura por defecto
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _fallbackTexture);

        GLint texLoc = glGetUniformLocation(_defaultShaderProgram, "texture0");
        if (texLoc != -1) {
            glUniform1i(texLoc, 0);
        }
    }

    //pillamos del shader el parametro
    GLint transformLoc = glGetUniformLocation(_defaultShaderProgram, "matrix");

    //gracias glm por ahorrarme tener que hacer estas cosas a mano
    //camara 
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)_width / (float)_height, nearPlane, farPlane);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

    //modelo
    glm::mat4 model = glm::mat4(1.0f);
    // lo movemos a la derecha para que no lo tape el panel de imgui
    model = glm::translate(model, glm::vec3(1.35f, 0.0f, 0.0f));
    // lo rotamos porque queda muy chulo
    if(_rotX || _rotY || _rotZ)
        model = glm::rotate(model, _modelRotationAngle, glm::vec3((float)_rotX, (float)_rotY, (float)_rotZ));
    _modelRotationAngle += _modelRotationSpeed * ImGui::GetIO().DeltaTime;
    // lo escalamos para que quepa en la ventana
    model = glm::scale(model, glm::vec3(_modelScaleFactor));

    //mandamos la matriz del modelo trasladado al shader para que en el calculo de la iluminacion se le aplique la iluminacion bien
    GLint modelMatLoc = glGetUniformLocation(_defaultShaderProgram, "modelMat");
    if (modelMatLoc != -1) {
        glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(model));
    }

    //multiplicamos todas las cosas y magia
    glm::mat4 mvp = projection * view * model;

    //mandar la matriz al shader
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    //bindear el vertex array object
    glBindVertexArray(_vao);

    //pum dibuja
    glDrawElements(GL_TRIANGLES, _bufferCount, GL_UNSIGNED_INT, 0);

    //hacemos que opengl apunte a todas las cosas por defecto para asegurarnos de no romper nada luego
    for (int i = 0; i < _activeTextures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + (GLenum)i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glUseProgram(0);
    glBindVertexArray(0);
}

void Window::setModelToBuffers(const ModelResults& result)
{
    //reiniciamos la rotacion del modelo al cambiar
    _modelRotationAngle = 0.0f;

    //posicion minima y maxima de la aabb del modelo
    glm::vec3 minBound, maxBound;
    //vector que guarda los vertices
    std::vector<Vertex> vertices;
    //vector que guarda los indices de como se unen los verticces
    std::vector<unsigned int> indices;
    //vector con los paths a las texturas del modelo
    std::vector<std::string> texturePaths;

    //esto es porque un fbx puede tener varias meshes y los indices empiezan para cada una en el 0,
    //enotnces necesitamos esto para evitar solapamientos raros
    unsigned int vertexOffset = 0;

    //solo miramos las cosas si el fbx no esta vacio
    if (!result.model.meshes.empty() && !result.model.meshes[0].vertexes.empty())
    {
        //esto es para calcular la aabb del modelo y hacer que quepa en la ventana (algunos de los de prueba no cabian y otros si)
        minBound = result.model.meshes[0].vertexes[0].position;
        maxBound = result.model.meshes[0].vertexes[0].position;

        //pillamos vertices e indices
        for (const auto& mesh : result.model.meshes)
        {
            for (const auto& v : mesh.vertexes)
            {
                //aqui ya metemos toda la informacion del vertice: posicion, normales, coordenadas de textura (esto importa para mas tarde)
                vertices.push_back(v);
                minBound = glm::min(minBound, v.position);
                maxBound = glm::max(maxBound, v.position);
            }

            for (int idx : mesh.indexes)
            {
                indices.push_back(idx + vertexOffset);
            }

            for (const auto& t : mesh.textures)
            {
                texturePaths.push_back(t.filePath);
            }

            vertexOffset += (unsigned int)mesh.vertexes.size();
        }
    }

    //calculamos el centro de la aabb
    _modelCenter = (minBound + maxBound) * 0.5f;
    //calculamos el lado mas largo para escalar el  modelo igual en todos los ejes
    float maxDim = std::max({ maxBound.x - minBound.x, maxBound.y - minBound.y, maxBound.z - minBound.z });
    //lo normalizamos acorde a la proporcion que ocupa de la ventana el visualizador de modelos (la mitad de la pantalla)
    _modelScaleFactor = 2.0f / maxDim; 

    //esto es cuanto tiene que leer opengl del buffer, el tamano cambiara dependiendo de lo que se le meta al buffer,
    //pero es NECESARIO actualizarlo a lo que tenga el modelo porque si no no va a dibujar todo
    _bufferCount = (GLsizei)indices.size();

    //borramos las texturas anteriores si las hubiera
    if (!_activeTextures.empty()) {
        glDeleteTextures((GLsizei)_activeTextures.size(), _activeTextures.data());
        _activeTextures.clear();
    }

    //por lo visto lo de que las texturas se carguen del reves es algo tipico y hay una funcion para ello lmao
    stbi_set_flip_vertically_on_load(true);

    for(const auto& texturePath : texturePaths )
    {
        //pum texturas (todo esto es mucha magia sacada de learnopengl)
        unsigned int texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);

        //ajustes de como wrappean las texturas y el filtro que se les aplica
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            //esto permite tener transparencias en base al numero de canales
            GLenum format = GL_RGB;
            if (nrChannels == 1)      format = GL_RED;
            else if (nrChannels == 4) format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            //nos guardamos la textura porque no queremos memory leaks
            _activeTextures.push_back(texId);
        }
        else {
#if _DEBUG
            std::cout << "Failed to load texture: " << texturePath << std::endl;
#endif
            //si falla borramos lo que hemos creado
            glDeleteTextures(1, &texId);
        }
        //una vez generada la textura podemos liberar esto
        stbi_image_free(data);
    }

    //bindear buffers
    //el vao lo que hace es guardar el vbo y el ebo de una forma mucho mas comoda para pasarselo a opengl
    glBindVertexArray(_vao);

    //vbo: Datos de los vértices
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    //ebo: Datos de los índices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    //menos mal que me dio por mirar cosas de opengl hace un año y pico, no se como habria hecho todo esto si no
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
    //indicarle al parametro del shader cada cuanto hay un atributo de posicion en el buffer
    GLint posAttrib = glGetAttribLocation(_defaultShaderProgram, "aPos");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(posAttrib);

    //lo mismo pero con las coordenadas de textura
    GLint texAttrib = glGetAttribLocation(_defaultShaderProgram, "aTexCoord");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(texAttrib);

    //lo mismo pero con  las normales
    GLint normAttrib = glGetAttribLocation(_defaultShaderProgram, "aNormal");
    if (normAttrib != -1) {
        glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(normAttrib);
    }

    // unbindeamos para que nada se rompa
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
    if(ImGui::Begin("Panel", &open, flags))
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

        ImGui::SameLine();
        ImGui::Checkbox("Visualizar normales", &_visualizeNormals);

        //checkboxes para los ejes en los que rota el modelo
        ImGui::Text("Ejes de rotacion");
        ImGui::SameLine();
        ImGui::Checkbox("x", &_rotX);
        ImGui::SameLine();
        ImGui::Checkbox("y", &_rotY);
        ImGui::SameLine();
        ImGui::Checkbox("z", &_rotZ);

        ImGui::SameLine();

        //slider de la velocidad de rotacion
        ImGui::SetNextItemWidth(_width/7.0f);
        ImGui::SliderFloat(" Velocidad de rotacion", &_modelRotationSpeed, 0.0, 10.0);

        if(ImGui::BeginChild("Desplegables", ImVec2(0.0, 0.0), true))
        {
            for (int i = 0; i < results.size(); i++) {
                createResultDropdown(results[i], i, buttonPressed);
            }
        }
        ImGui::EndChild();
        
    }
    //SIEMPRE hay que llamar a un end por cada begin
    ImGui::End();
}

void Window::createResultDropdown(const ModelResults& result, int index, bool buttonPressed)
{
    //podemos pasarle la direccion de memoria del resultado a imgui como id para que cada dropdown sea independiente de los demas
    ImGui::PushID(&result);

    //si el modelo ha pasado o no todos los tests
    if (!result.index.has_value())
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "(Pendiente)");
    else
        ImGui::TextColored(result.allTestsPassed ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1), result.allTestsPassed ? "(Aprobado)" : "(Suspenso)");

    ImGui::SameLine();
    
    if (buttonPressed)
        ImGui::SetNextItemOpen(!_collapsed);
    //quizas un poco feo tener esto asi, pero imgui solo dibuja los contenidos si el header esta abierto,
    //y como queremos tener un hover por si el nombre del archivo es demasiado largo, tenemos que hacerlo asi
    //(si estuviera dentro de la condicion de abierto solo se ejecutaria cuando el header estuviera abierto)
    bool abierto = ImGui::CollapsingHeader(_modelNames[index].c_str());
    if(ImGui::IsItemHovered())
        ImGui::SetTooltip(_modelNames[index].c_str());

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