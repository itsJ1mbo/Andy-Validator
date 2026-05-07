#include "AndyValidator_FBX/FBX.h"

#include <format>
#include <iostream>

FBX& FBX::instance()
{
    if (!_instance)
    {
        _instance = std::unique_ptr<FBX>(new FBX());
    }

    return *_instance;
}

bool FBX::init()
{
	if (_instance)
    {
        return initSdk();
    }
    else
    {
        return false;
    }
}

void FBX::free() const
{
    _scene->Destroy();
    _importer->Destroy();
    _ioSettings->Destroy();
    _sdkManager->Destroy();
}

bool FBX::initSdk()
{
    _sdkManager = FbxManager::Create();
    if (!_sdkManager)
    {
		std::cout << "Error al crear el SDK de FBX\n" ;
        return false;
    }

    _ioSettings = FbxIOSettings::Create(_sdkManager, IOSROOT);
	if (!_ioSettings)
    {
        std::cout << "Error al crear las IOSettings del SDK de FBX\n";
        return false;
    }
    _sdkManager->SetIOSettings(_ioSettings);
 
	_importer = FbxImporter::Create(_sdkManager, "");
    if (!_importer)
    {
		std::cout << "Error al crear el importador del SDK de FBX\n";
        return false;
    }

    _scene = FbxScene::Create(_sdkManager, "Scene");
    if (!_scene)
    {
        std::cout << "Error al crear la escena del SDK de FBX\n";
        return false;
    }


    return true;
}

bool FBX::import(const std::list<std::string>& filePaths) const
{
    if (!_importer->Initialize(filePaths.back().c_str(), -1, _sdkManager->GetIOSettings()))
    {
        std::cout << std::format("Error al inicializar el importador del SDK de FBX: {}", _importer->GetStatus().GetErrorString());
        return false;
    }

    if (!_importer->Import(_scene))
    {
	    std::cout << std::format("Error al importar la escena del SDK de FBX: {}", _importer->GetStatus().GetErrorString());
		return false;
    }

    return true;
}
