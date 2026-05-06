#include "AndyValidator_Core/FBXReader.h"

namespace fs = std::filesystem;

FBXReader* FBXReader::_instance = nullptr;

FBXReader* FBXReader::instance()
{
    if (!_instance)
    {
        _instance = new FBXReader();
    }

    return _instance;
}

bool FBXReader::init()
{
    if (_instance) 
    {
        return _instance->initDirectory();
    }
    else 
    {
        return false;
    }
}

void FBXReader::readModels()
{
    for (const auto& archivo : _directory) 
    {
        if (archivo.is_regular_file() && archivo.path().extension() == ".fbx") 
        {
            _fbxPaths.push_back(archivo.path().string());
        }
    }
}

bool FBXReader::initDirectory()
{
    _path = "./FBXs";

    if (fs::exists(_path) && fs::is_directory(_path))
    {
        _directory = fs::directory_iterator(_path);
    }
    else
    {
        return false;
    }

    return true;
}
