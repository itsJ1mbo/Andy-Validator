#include <iostream>
#include "AndyValidator_Core/FBXReader.h"
#include "AndyValidator_Window/Window.h"

int main() {
    
    if (!FBXReader::instance().init()) {
        return 1;
    }

    /*for (const auto& archivo : fs::directory_iterator(rutaCarpeta)) {

        if (archivo.is_regular_file() && archivo.path().extension() == ".fbx") {
            fbxEncontrado = true;
            rutaArchivoFBX = archivo.path().string();

        }
    }
    if (fbxEncontrado) {
        for (int i = 0; i < 50; i++) cout << "=";
        cout << "\nArchivo FBX encontrado en: " << rutaArchivoFBX << "\n";
        for (int i = 0; i < 50; i++) cout << "=";


        cout << "\nBarigadumdumdum (Hacer cosas con el fbx)\n";
        for (int i = 0; i < 50; i++) cout << "=";

    }
    else {
        cout << "\nNo hay .fbx en la carpeta.\n";
    }*/

    Window* ventanaEpica = new Window();

    while (!ventanaEpica->shouldWindowClose()) {
        ventanaEpica->updateWindow();
    }

    delete ventanaEpica;

    return 0;
}