#include "AndyValidator_FBX/ValidatorManager.h"
#include "AndyValidator_FBX/Validation.h"

void ValidatorManager::startValidationTask(const std::vector<FbxScene*>& fbxs)
{
    if (_isRunning) return;

    _isRunning = true;
    _hasNewData = false;

	// Seccion critica bloqueante
    {
        std::scoped_lock lock(_mutex);
        _resultsBuffer.clear();
    }

    _thread = std::jthread(&ValidatorManager::validationTask, this, fbxs);
}

void ValidatorManager::validationTask(const std::vector<FbxScene*>& fbxs)
{
    for (const auto& model : fbxs) {

        ValidationResults fileResults;

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
