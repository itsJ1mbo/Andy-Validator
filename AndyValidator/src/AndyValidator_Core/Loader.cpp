#include "AndyValidator_Core/Loader.h"

#include <iostream>
#include <format>
#include <fstream>

namespace fs = std::filesystem;

Loader::Loader()
{
	initDirectory();
}

void Loader::read()
{
    for (const auto& file : _directory) 
    {
        if (file.is_regular_file() && file.path().extension() == ".fbx")
        {
#if _DEBUG
            std::cout << std::format("FBX \"{}\" registrado con exito\n", file.path().filename().string());
#endif
            _fbxEntries.push_back(file.path().string());
        }
        else if (file.is_regular_file() && file.path().extension() == ".cfg")
        {
            readConfig(file.path().string());
        }
    }

    if (!_configRead)
    {
        createConfig();
    }
}

void Loader::initDirectory()
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
        else
        {
            read();
        }
    }
}

void Loader::readConfig(const std::string& path)
{
	std::ifstream file(path);
    if (!file.is_open())
    {
#if _DEBUG
		std::cout << std::format("No se pudo abrir el archivo de configuracion: {}\n", path);
#endif
		return;
    }

    std::string line;
    while (std::getline(file, line)) 
    {
        if (line.empty() || line[0] == '#') continue;

        size_t equal = line.find('=');
        if (equal == std::string::npos) continue;

        std::string key = line.substr(0, equal);
        std::string value = line.substr(equal + 1);

        key.erase(std::ranges::remove_if(key, isspace).begin(), key.end());

        if (key == "MAX_POLYGONS") 
        {
            _cfg.polygons = std::stoi(value);
        }
		else if (key == "UNREAL_ENGINE")
        {
			_cfg.unreal = value == "true";
        }
    }

    file.close();
	_configRead = true;
}

void Loader::createConfig()
{
    std::ofstream file(_path.append("\\validator.cfg"));
    if (!file.is_open())
    {
#if _DEBUG
        std::cout << "No se pudo crear el archivo de configuracion\n";
#endif
        return;
    }

    file << std::format("MAX_POLYGONS = {}\n", _cfg.polygons);
    file << std::format("UNREAL_ENGINE = {}\n", _cfg.unreal ? "true" : "false");

    file.close();
}