# Andy-Validator
Software de validacion de modelos 3D de formato FBX para videojuegos

## Instrucciones
### Instalación del FBX SDK
1. Descargar el instalador del sdk versión 2020.3.9 desde [la web oficial](https://aps.autodesk.com/developer/overview/fbx-sdk)
2. Instalarlo en la carpeta dependencies, debería quedar algo como "dependencies/2020.3.9/"

## Arquitectura
### Window
Proyecto que gestiona y crea la ventana. Proporciona las herramientas para crear elementos de la UI
### Fbx
Proyecto que realiza la validacion a traves del sdk de autodesk. El punto de entrada recibe la lista de fbx a validar y devuelve los resultados al Core
### Core
Proyecto que gestiona la aplicacion y su bucle. Crea la UI, lee los fbx y se los pasa a autodesk y gestiona los resultados devueltos

## Librerias
pum

![Cuantas librerias hay en el proyecto?](dependencies.png)












