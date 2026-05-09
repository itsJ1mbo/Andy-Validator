#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>

class FbxScene;
class Validation;
struct ValidationResults;

class ValidatorManager 
{
public:
    void startValidationTask(const std::vector<FbxScene*>& fbxs);
    bool isRunning() const { return _isRunning; }

    std::vector<ValidationResults> checkNewResults();

private:
    void workerTask(const std::vector<FbxScene*>& fbxs);

    std::vector<std::unique_ptr<Validation>> _validations;

    std::jthread _thread;
    std::mutex _mutex;
    std::vector<ValidationResults> _resultsBuffer;

    std::atomic<bool> _isRunning{ false };
    std::atomic<bool> _hasNewData{ false };
};

