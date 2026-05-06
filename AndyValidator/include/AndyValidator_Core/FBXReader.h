#pragma once

#include <memory>
#include <string> 
#include <filesystem>

class FBXReader
{
public:
	static FBXReader* instance();

	bool init();

	void readModels();

private:
	FBXReader() = default;
	~FBXReader() = default;

	bool initDirectory();

	static std::unique_ptr<FBXReader> _instance;

	std::string _path;
	std::filesystem::directory_iterator _directory;
};

