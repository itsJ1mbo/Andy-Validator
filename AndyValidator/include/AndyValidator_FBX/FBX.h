#pragma once

#include <memory>
#include <vector>

#include "AndyValidator_FBX/Validation.h"

class ValidatorManager;

class FBX
{
public:
	~FBX();
	static FBX& instance();

	bool init();
	void free() const;

	void start(const std::vector<std::string>& files) const;
	std::vector<ValidationResults> checkNewResults() const;

private:
	FBX() = default;
	FBX(const FBX& in) = delete;
	FBX& operator=(const FBX& in) = delete;
	FBX(const FBX&& in) = delete;
	FBX& operator=(const FBX&& in) = delete;

	bool initSdk();
	FbxScene* import(const std::string& file) const;

	inline static std::unique_ptr<FBX> _instance;

	std::unique_ptr<ValidatorManager> _manager;

	FbxManager* _sdkManager;
	FbxIOSettings* _ioSettings;
	FbxImporter* _importer;
};

