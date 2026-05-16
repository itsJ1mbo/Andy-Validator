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
#include <iostream>
#include <filesystem>
#include <utility>

const char* vertexShaderSource = "#version 120\n"
"attribute vec3 aPos;\n"
"attribute vec2 aTexCoord;\n" // este para setearlo
"varying vec2 TexCoord;\n"     // este para pasarselo al shader de fragmentos
"uniform mat4 matrix;\n"
"void main()\n"
"{\n"
"    gl_Position = matrix * (vec4(aPos, 1.0));\n"
"    TexCoord = aTexCoord;\n"
"}\n";

const char* fragmentShaderSource = "#version 120\n"
"varying vec2 TexCoord;\n"
"uniform sampler2D texture0;\n"
"uniform sampler2D texture1;\n"
"void main()\n"
"{\n"
"    float ambientStrength = 1.0;\n"
"    vec3 lightColor = vec3(1.0, 1.0, 1.0);\n"
"    vec3 ambient = ambientStrength * lightColor;\n"
"    vec4 color0 = texture2D(texture0, TexCoord);\n"
"    vec4 color1 = texture2D(texture1, TexCoord);\n"
"    vec4 objectColor = color0;\n" //si la segunda textura tiene cosas entonces la blendeamos, si no usamos solo la primera
"    if (color1.a > 0.0) {\n"
"        objectColor = mix(color0, color1, 0.5);\n"
"    }\n"
"    vec3 result = ambient * objectColor.rgb;\n"
"    gl_FragColor = vec4(result, objectColor.a);\n"
"}\n";


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Window::Window(int width, int height) : _width(width), _height(height), _glfwWindow(nullptr), _collapsed(true), _bufferCount(0), _vao(0), _vbo(0), _ebo(0), _shaderProgram(0), _modelRotationAngle(0), _modelRotationSpeed(1.0)
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
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
#if _DEBUG
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
#endif
        return false;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    //lo mismo pero con el de fragmentos
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
#if _DEBUG
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
#endif
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
#if _DEBUG
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
#endif
        return false;
    }

    //los borramos porque ya estan en el shader program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //depth test para que no se solapen mal
    glEnable(GL_DEPTH_TEST);

    //le metemos el culling para las normales 
    glEnable(GL_CULL_FACE);
    //decidimos no ocultar ninguno de los dos lados de la cara para poder visualizar bien si hay alguna normal flippeada
    glCullFace(GL_NONE);
    
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

    //unbindeamos la textura
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

    //creamos el panel de imgui (si, hay que crearlo en cada frame, imgui funciona asi)
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
            GLint texLoc = glGetUniformLocation(_shaderProgram, uniformName.c_str());

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

        GLint texLoc = glGetUniformLocation(_shaderProgram, "texture0");
        if (texLoc != -1) {
            glUniform1i(texLoc, 0);
        }
    }

    //pillamos del shader el parametro
    GLint transformLoc = glGetUniformLocation(_shaderProgram, "matrix");

    //gracias glm por ahorrarme tener que hacer estas cosas a mano
    float fov = 45.0;
    float nearPlane = 0.1;
    float farPlane = 1000;
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)_width / (float)_height, nearPlane, farPlane);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

    glm::mat4 model = glm::mat4(1.0f);
    //lo movemos a la derecha para que no lo tape el panel de imgui (si es un numero magico muy raro puesto a base de prueba y error)
    model = glm::translate(model, glm::vec3(1.35f, 0.0f, 0.0f));
    //lo rotamos porque queda muy chulo
    model = glm::rotate(model, _modelRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    _modelRotationAngle += _modelRotationSpeed * ImGui::GetIO().DeltaTime;
    //lo escalamos para que quepa en la ventana
    model = glm::scale(model, glm::vec3(_modelScaleFactor));

    //multiplicamos todas las cosas y magia
    glm::mat4 mvp = projection * view * model;

    //mandar la matriz al shader
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    //bindear el vertex array object
    glBindVertexArray(_vao);

    //pum dibuja
    glDrawElements(GL_TRIANGLES, _bufferCount, GL_UNSIGNED_INT, 0);

    //limpiamos esto para que imgui no se rompa
    for (size_t i = 0; i < _activeTextures.size(); ++i) {
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

    glm::vec3 minBound, maxBound;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
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

    //calculamos el centro de la esfera
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
            //esto permite tener transparencias en base 
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

    GLint texAttrib = glGetAttribLocation(_shaderProgram, "aTexCoord");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(texAttrib);

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

        ImGui::SameLine();

        ImGui::SetNextItemWidth(_width/7.0);
        ImGui::SliderFloat(" Velocidad de rotacion", &_modelRotationSpeed, 0.0, 10.0);

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