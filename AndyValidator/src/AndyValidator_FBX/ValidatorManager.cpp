#include "AndyValidator_FBX/ValidatorManager.h"
#include "AndyValidator_FBX/Validation.h"

ValidatorManager::ValidatorManager()
{
    // _validations.push_back(std::make_unique<DummyValidation>());
}

void ValidatorManager::startValidationTask(const std::vector<std::string>& fbxs, const std::function<FbxScene*(const std::string&)>& loader)
{
    if (_isRunning) return;

    _isRunning = true;
    _hasNewData = false;

	// Seccion critica bloqueante
    {
        std::scoped_lock lock(_mutex);
        _resultsBuffer.clear();
    }

    _thread = std::jthread([this, fbxs, loader](const std::stop_token& stopToken) {
        this->validationTask(stopToken, fbxs, loader);
    });
}

void ValidatorManager::stopValidationTask()
{
    if (_isRunning)
    {
        _thread.request_stop();
        _thread.join();
	}
}

void ValidatorManager::validationTask(const std::stop_token& stopToken, const std::vector<std::string>& fbxs, const std::function<FbxScene*(const std::string&)>& loader)
{
    for (size_t i = 0; i < fbxs.size(); ++i) 
    {
        if (stopToken.stop_requested()) break;

		FbxScene* model = loader(fbxs[i]);

		if (model)
        {
	        ValidationResults fileResults;
            fileResults.index = i;

	        for (const auto& validation : _validations)
	        {
	        	validation->validate(model, fileResults);
	        }

        	// Seccion critica bloqueante
	        {
	        	std::scoped_lock lock(_mutex);
	        	_resultsBuffer.push_back(fileResults);
	        	_hasNewData = true;
	        }

            model->Destroy();
        }
    }

    _isRunning = false;
}

std::vector<ValidationResults> ValidatorManager::checkNewResults()
{
    std::vector<ValidationResults> newResults;

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
