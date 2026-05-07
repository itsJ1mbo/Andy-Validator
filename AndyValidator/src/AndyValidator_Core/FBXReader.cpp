#include "AndyValidator_Core/FBXReader.h"
#include <iostream>
#include <format>
#include "AndyValidator_FBX/FBX.h"

namespace fs = std::filesystem;

FBXReader::FBXReader()
{
	initDirectory();
}

void FBXReader::readModels()
{
    for (const auto& file : _directory) 
    {
        if (file.is_regular_file() && file.path().extension() == ".fbx")
        {
#if _DEBUG
            std::cout << std::format("FBX \"{}\" registrado con exito\n", file.path().filename().string());
#endif
            _fbxEntries.push_back(file);
            FBX::instance().import(file.path().string());
        }
    }
}

void FBXReader::initDirectory()
{
    _path = "..\\..\\FBXs";

    if (!fs::exists(_path) || !fs::is_directory(_path))
    {
        std::cout << "No hay directorio de FBXs\n";
    }
    else 
    {
        _directory = fs::directory_iterator(_path);
        fs::directory_iterator end;

        if (_directory == end)
        {
            std::cout << "El directorio de FBXs esta vacio\n";
        }
    }
}
