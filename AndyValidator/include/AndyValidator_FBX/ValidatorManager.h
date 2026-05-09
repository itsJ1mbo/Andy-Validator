#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <stop_token>
#include <functional>

#include "AndyValidator_FBX/Validation.h"

class ValidatorManager 
{
public:
    ValidatorManager();

    void startValidationTask(const std::vector<std::string>& fbxs, const std::function<FbxScene*(const std::string&)>& loader);
    void stopValidationTask();
    bool isRunning() const { return _isRunning; }

    std::vector<ValidationResults> checkNewResults();

private:
    void validationTask(const std::stop_token& stopToken, const std::vector<std::string>& fbxs, const std::function<FbxScene*(const std::string&)>& loader);

    std::vector<std::unique_ptr<Validation>> _validations;

    std::jthread _thread;
    std::mutex _mutex;
    std::vector<ValidationResults> _resultsBuffer;

    std::atomic<bool> _isRunning{ false };
    std::atomic<bool> _hasNewData{ false };
};

