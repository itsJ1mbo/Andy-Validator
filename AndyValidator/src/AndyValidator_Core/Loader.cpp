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

    std::cout << "Buscando en: " << fs::absolute(_path) << std::endl;

    if (!fs::exists(_path) || !fs::is_directory(_path))
    {
#if _DEBUG
        std::cout << "No hay directorio de FBXs\n";
#endif
    }
    else 
    {
        _directory = fs::directory_iterator(_path);
        fs::directory_iterator end;

        if (_directory == end)
        {
#if _DEBUG
            std::cout << "El directorio de FBXs esta vacio\n";
#endif
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
        value.erase(std::ranges::remove_if(value, isspace).begin(), value.end());

        if (key == "MAX_POLYGONS") 
        {
            _cfg.polygons = std::stoi(value);
        }
		else if (key == "UNREAL_ENGINE")
        {
			_cfg.unreal = value == "true";
        }
        else if (key == "NAMING_NOMENCLATURE")
        {
            if (value == "UpperCamelCase") _cfg.naming = NamingNomenclature::UpperCamelCase;
            else if (value == "lowerCamelCase") _cfg.naming = NamingNomenclature::lowerCamelCase;
            else if (value == "Upper_Snake_Case") _cfg.naming = NamingNomenclature::Upper_Snake_Case;
            else if (value == "lower_snake_case") _cfg.naming = NamingNomenclature::lower_snake_case;
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

    std::string namingStr = "UpperCamelCase";
    switch (_cfg.naming)
    {
        case NamingNomenclature::UpperCamelCase:   namingStr = "UpperCamelCase"; break;
        case NamingNomenclature::lowerCamelCase:   namingStr = "lowerCamelCase"; break;
        case NamingNomenclature::Upper_Snake_Case: namingStr = "Upper_Snake_Case"; break;
        case NamingNomenclature::lower_snake_case: namingStr = "lower_snake_case"; break;
    }

    file << std::format("MAX_POLYGONS = {}\n", _cfg.polygons);
    file << std::format("UNREAL_ENGINE = {}\n", _cfg.unreal ? "true" : "false");
    file << std::format("NAMING_CONVENTION = {}\n", namingStr);

    file.close();
}