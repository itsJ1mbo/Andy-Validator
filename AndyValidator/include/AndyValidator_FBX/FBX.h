#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include <fbxsdk.h>

#include "ModelData.h"

class ImportManager;
class ValidatorManager;

class FBX
{
public:
	~FBX();
	static FBX& instance();

	bool init();
	void free();

	void start(const std::vector<std::string>& files, const Config config);

	std::vector<ModelResults> checkNewResults();
	Config getConfig() const
	{
		return _cfg;
	}

private:
	FBX() = default;
	FBX(const FBX& in) = delete;
	FBX& operator=(const FBX& in) = delete;
	FBX(const FBX&& in) = delete;
	FBX& operator=(const FBX&& in) = delete;

	bool initSdk();
	void processTask(const std::stop_token& stopToken, const std::vector<std::string>& files);
	void stop();

	inline static std::unique_ptr<FBX> _instance;

	std::unique_ptr<ValidatorManager> _validatorManager;
	std::unique_ptr<ImportManager> _importManager;

	FbxManager* _sdkManager;
	FbxIOSettings* _ioSettings;

	std::jthread _thread;
	std::mutex _mutex;
	std::vector<ModelResults> _resultsBuffer;

	std::atomic<bool> _isRunning{ false };
	std::atomic<bool> _hasNewData{ false };

	Config _cfg;
};

