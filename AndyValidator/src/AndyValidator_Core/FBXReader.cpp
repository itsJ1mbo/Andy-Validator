#include "FBXReader.h"  

namespace fs = std::filesystem;

FBXReader* FBXReader::instance()
{
    return _instance.get();
}

bool FBXReader::init()
{
    _instance = std::make_unique<FBXReader>();

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
        if (archivo.is_regular_file() && archivo.path().extension() == ".fbx") {
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
