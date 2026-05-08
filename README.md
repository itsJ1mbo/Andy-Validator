# Andy-Validator
Software de validacion de modelos 3D de formato FBX para videojuegos

## Índice
1. [Instrucciones](#instrucciones)
2. [Concept](#concept-art)
3. [Arquitectura](#arquitectura)
4. [Librerias](#clibrerias)

## Instrucciones
### Instalación del FBX SDK
1. Descargar el instalador del sdk versión 2020.3.9 desde [la web oficial](https://aps.autodesk.com/developer/overview/fbx-sdk)
2. Instalarlo en la carpeta dependencies, debería quedar algo como "dependencies/2020.3.9/"  

Recuerdo poner el directorio de trabajo del proyecto principal en $(TargetDir)

## Concept art
**AVISO: HE HABALDO CON GUILLE TENER UN BOTON VALIDAR ES ILEGAL SE HACE TODO NADA MAS ABRIR LA APLICACION. NO HAY BOTON**
### Opcion 1
![Opcion 1](opcion1.png)
### Opcion 2
![Opcion 2](opcion2.png)
   
## Arquitectura
### Window
Proyecto que gestiona y crea la ventana. Proporciona las herramientas para crear elementos de la UI
### FBX
Proyecto que realiza la validacion a traves del sdk de autodesk. El punto de entrada inicializa la libreria y carga los modelos.
#### Validaciones
La clase Validation es virtual y de ella heredara cada tipo de validacion distinta, implementando el metodo validate() segun cada caso
```
class Validation
{
public:
	virtual ~Validation() = default;

	virtual void validate(const FbxScene* fbx, ValidationResults& results) = 0;
};
```
El FbxScene es el modelo, la libreria los llama escenas pero es un unico modelo. ValidationResults es un struct que tendrá un booleano por cada validacion. Se pasa por referencia porque cada implementacion de validate() pondrá a true el booleano correspondiente a su validación si el modelo lo cumple (hay un struct por modelo)
```
struct ValidationResults
{
	// bool validacionX
	// bool validacionY
	// bool validacionZ
	// etc
};
```
#### Comunicación
Habrá un "ValidationManager" encargado de iterar por cada modelo y llamando a todos los validate() con cada uno. Guardará los resultados de la validacion en un vector que leerá el Core para mostrarlos en la ventana. También gestionará el hilo trabajador encargado de realizar todas las validaciones mientras el hilo principal renderiza (sí, hay dos hilos lo voy a hacer yo no problem)
### Core
Proyecto que gestiona la aplicacion y su bucle. Crea la UI y lee los modelos mientras FBX los carga, gestiona los resultados devueltos por el proyecto FBX para mostrarlos en la ventana

## Librerias
pum

![Cuantas librerias hay en el proyecto?](dependencies.png)











