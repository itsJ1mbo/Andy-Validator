#include "AndyValidator_FBX/FBX.h"
#include "AndyValidator_FBX/ValidatorManager.h"

#include <format>
#include <iostream>

FBX::~FBX() = default;
	
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
		_manager = std::make_unique<ValidatorManager>();
        return initSdk();
    }
    else
    {
        return false;
    }
}

void FBX::free() const
{
    _manager->stopValidationTask();
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

    return true;
}

FbxScene* FBX::import(const std::string& file) const
{
	if (!_importer->Initialize(file.c_str(), -1, _sdkManager->GetIOSettings()))
	{
		std::cout << std::format("Error al inicializar el archivo FBX: {}, {}", file, _importer->GetStatus().GetErrorString());
        return nullptr;
	}

	FbxScene* scene = FbxScene::Create(_sdkManager, "Scene");
	if (!scene)
	{
		std::cout << "Error al crear la escena del SDK de FBX\n";
        return nullptr;
	}

	if (!_importer->Import(scene))
	{
		std::cout << std::format("Error al importar la escena del SDK de FBX: {}", _importer->GetStatus().GetErrorString());
        scene->Destroy();
        return nullptr;
	}
	
    return scene;
}

void FBX::start(const std::vector<std::string>& files) const
{
    auto loader = [this](const std::string& ruta) -> FbxScene* {
        return this->import(ruta);
    };

    _manager->startValidationTask(files, loader);
}

std::vector<ValidationResults> FBX::checkNewResults() const
{
    return _manager->checkNewResults();
}
