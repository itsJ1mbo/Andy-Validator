#include "AndyValidator_FBX/FBX.h"
#include "AndyValidator_FBX/ValidatorManager.h"
#include "AndyValidator_FBX/ImportManager.h"

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
		_validatorManager = std::make_unique<ValidatorManager>();
        return initSdk();
    }
    else
    {
        return false;
    }
}

void FBX::free()
{
    stop();

    _ioSettings->Destroy();
    _sdkManager->Destroy();
}

bool FBX::initSdk()
{
    _sdkManager = FbxManager::Create();
    if (!_sdkManager)
    {
#if _DEBUG
		std::cout << "Error al crear el SDK de FBX\n" ;
#endif
        return false;
    }

    _ioSettings = FbxIOSettings::Create(_sdkManager, IOSROOT);
	if (!_ioSettings)
    {
#if _DEBUG
        std::cout << "Error al crear las IOSettings del SDK de FBX\n";
#endif
        return false;
    }
    _sdkManager->SetIOSettings(_ioSettings);

    return true;
}

void FBX::processTask(const std::stop_token& stopToken, const std::vector<std::string>& files)
{
    for (size_t i = 0; i < files.size(); ++i)
    {
        if (stopToken.stop_requested()) break;

        FbxScene* model = _importManager->import(files[i], _sdkManager);

        if (model)
        {
            Results fileResults;
            fileResults.index = i;

            _validatorManager->runValidations(model, fileResults);

			fileResults.model = _importManager->processModel(model, _sdkManager);

#if _DEBUG
			std::cout << std::format("Archivo {} procesado\n", files[i]);
#endif

            // Seccion critica bloqueante
            {
                std::scoped_lock lock(_mutex);
                _resultsBuffer.push_back(fileResults);
                _hasNewData = true;
            }

            model->Destroy();
        }
    }
}

void FBX::start(const std::vector<std::string>& files, const Config config)
{
    if (_isRunning) return;

	_cfg = config;
    _isRunning = true;
    _hasNewData = false;

    // Seccion critica bloqueante
    {
        std::scoped_lock lock(_mutex);
        _resultsBuffer.clear();
    }

    _thread = std::jthread([this, files](const std::stop_token& stopToken) {
        this->processTask(stopToken, files);
    });
}

void FBX::stop()
{
    if (_isRunning)
    {
        _thread.request_stop();
        _thread.join();
    }
}

std::vector<Results> FBX::checkNewResults()
{
    std::vector<Results> newResults;

    if (_hasNewData)
    {
        // Seccion critica no bloqueante
        if (_mutex.try_lock())
        {
            std::scoped_lock lock(std::adopt_lock, _mutex);

            std::swap(newResults, _resultsBuffer);
            _hasNewData = false;
        }
    }

    return newResults;
}
