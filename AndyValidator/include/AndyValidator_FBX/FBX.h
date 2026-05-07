#pragma once

#include <memory>
#include <fbxsdk.h>
#include <list>

class FBX
{
public:
	static FBX& instance();

	bool init();
	void free() const;

	bool import(const std::list<std::string>& filePaths) const;

private:
	FBX() = default;
	FBX(const FBX& in) = delete;
	FBX& operator=(const FBX& in) = delete;
	FBX(const FBX&& in) = delete;
	FBX& operator=(const FBX&& in) = delete;

	bool initSdk();

	inline static std::unique_ptr<FBX> _instance;

	FbxManager* _sdkManager;
	FbxIOSettings* _ioSettings;
	FbxImporter* _importer;
	FbxScene* _scene;
};

