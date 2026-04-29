#include <iostream>
#include <filesystem>
#include <string>

using namespace std;
namespace fs = std::filesystem;

int main() {
    string rutaCarpeta = "../../FBXs";

    bool fbxEncontrado = false;
    string rutaArchivoFBX = "";
    if (fs::exists(rutaCarpeta) && fs::is_directory(rutaCarpeta)) {

        for (const auto& archivo : fs::directory_iterator(rutaCarpeta)) {

            if (archivo.is_regular_file() && archivo.path().extension() == ".fbx") {
                fbxEncontrado = true;
                rutaArchivoFBX = archivo.path().string();

            }
        }
    }
    else {
        for (int i = 0; i < 50; i++) cout << "=";
        cout << "\nError: La carpeta no existe o no es valida\n";
        for (int i = 0; i < 50; i++) cout << "=";
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
    }

    return 0;
}