#include <iostream>
#include <filesystem>
#include <string>
#include "FBXReader.h"

using namespace std;
namespace fs = std::filesystem;

int main() {
    
    if (!FBXReader::instance()->init()) {
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

    return 0;
}