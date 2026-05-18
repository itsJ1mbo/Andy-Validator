# Andy Validator
Software de validacion de modelos 3D de formato FBX para videojuegos

## Índice
1. [Instrucciones](#instrucciones)
2. [Concept](#concept-art)
3. [Arquitectura](#arquitectura)
4. [Librerias](#clibrerias)
5. [Terceros](#terceros)

## Instrucciones
### Desgargar ejecutable
1. Desde la página de Releases descargar la última disponible
2. Descomprimir el zip y mover el EXE y el CFG a la misma carpeta que los modelos
3. (Opcional) Modificar los datos del archivo CFG según necesidad
4. Ejcutar el EXE
### Compilar desde la fuente
1. Descargar el repositorio
2. Instalar el FBX SDK
    -  Descargar el instalador del sdk versión 2020.3.9 desde [la web oficial](https://aps.autodesk.com/developer/overview/fbx-sdk)
    - Instalarlo en la carpeta dependencies de la solución de VS, debería quedar algo como "dependencies/2020.3.9/"  
3. Abrir la solución y compilar
4. Mover el binario (EXE) generado a la carpeta de los modelos
5. (Opcional) crear el archivo validator.cfg en la misma carpeta y configurarlo a gusto
6. Ejecutar el EXE

## Concept art
### Ventana
![Opcion 1](opcion1.png)
   
## Arquitectura
### Window
Proyecto que gestiona y crea la ventana. Proporciona las herramientas para crear elementos de la UI
### FBX
Proyecto que realiza la validacion a traves del sdk de autodesk. El punto de entrada inicializa la libreria y gestiona el hilo secundario que corre la importacion y validaciones de cada modelo.
#### Validaciones
La clase Validation es virtual y de ella heredara cada tipo de validacion distinta, implementando el metodo validate() segun cada caso
```cpp
class Validation
{
public:
	virtual ~Validation() = default;

	virtual void validate(const FbxScene* fbx, ModelResults& results) = 0;
};
```
El FbxScene es el modelo, la libreria los llama escenas pero es un unico modelo. ModelResults es un struct que tendrá un vector de ValidationResult, uno por cada validacion. Se pasa por referencia porque cada implementacion de validate() gestionará cada resultado de la validacion (ValidationType y passed) si el modelo lo cumple (hay un struct por modelo)
```cpp
enum ValidationType { None, TestX, TestY /* etc... */};

struct ValidationResult
{
    ValidationType type;
    std::string description;
    bool passed;
};

struct ModelResults
{
    std::optional<size_t> index;
    std::string fileName;
    ModelData model;
    bool allTestsPassed = true;
    std::vector<ValidationResult> validations;
};
```
El ValidatorManager es el encargado de iterar por cada modelo llamando a todos los validate() de cada uno. Guardará los resultados de la validacion en un vector que leerá el Core para mostrarlos en la ventana. 
```cpp
class ValidatorManager 
{
public:
    ValidatorManager();

    void runValidations(const FbxScene* scene, ModelResults& results) const;

private:
    std::vector<std::unique_ptr<Validation>> _validations;
};
```
#### Importacion
ImportManager importa cada modelo a una FbxScene que luego FBX pasa al ValidatorManager, tras las validaciones el ImportManager traduce los datos del modelo a una estructura que OpenGL pueda entender
```cpp
struct Vertex 
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct TextureData 
{
    std::string filePath;
    std::string type;
};

struct MeshData 
{
    std::vector<Vertex> vertexes;
    std::vector<int> indexes;
    std::vector<TextureData> textures;
};

struct ModelData 
{
    std::vector<MeshData> meshes;
};
```
### Core
Proyecto que gestiona la aplicacion y su bucle. Llama al render y lee los modelos que luego FBX carga, gestiona los resultados devueltos por el proyecto FBX para mostrarlos en la ventana.
```cpp
void Application::run()
{
    FBX::instance().start(_loader->getModelPaths(), _loader->getConfig()); // Inicia hilo trabajador

    Window::instance().setModelNames(_loader->getModelPaths());

    while (!Window::instance().shouldWindowClose())
    {
        const auto results = FBX::instance().checkNewResults();
        for (const auto& result : results)
    	{
            _results[*result.index] = result;
        }

        Window::instance().updateWindow();
    }
}
```
Lee de la misma carpeta un archivo validator.cfg que tiene datos dependientes del proyecto específico. El usuario debe cambiarlo segun la necesidad. Si no encunetra el archivo lo crea con valores por defecto
```cpp
struct Config
{
    int polygons = 10000;
    bool unreal = false;
    NamingNomenclature naming = NamingNomenclature::UpperCamelCase;
    double texelDensityTolerance = 1.30;
};
```

## Librerias
pum (y glm)

![Cuantas librerias hay en el proyecto?](dependencies.png)

## Terceros
El ejecutable final contiene el SDK de Autodesk® FBX® de forma estática. El uso de este SDK está sujeto a los términos de licencia de Autodesk. Nuestro código interactúa con este SDK pero es independiente de él.










