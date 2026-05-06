#include "AndyValidator_Core/FBXReader.h"

namespace fs = std::filesystem;

const FBXReader& FBXReader::instance()
{
    if (!_instance)
    {
        _instance = std::unique_ptr<FBXReader>(new FBXReader());
    }

    return *_instance;
}

bool FBXReader::init() const
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
    for (const auto& file : _directory) 
    {
        if (file.is_regular_file() && file.path().extension() == ".fbx")
        {
            _fbxPaths.push_back(file.path().string());
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
