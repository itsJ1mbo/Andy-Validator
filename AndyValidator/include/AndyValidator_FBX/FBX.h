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

	void import(const std::string& file);

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
	std::list<FbxScene*> _scene;
};

