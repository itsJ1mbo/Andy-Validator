#pragma once

#include <string> 
#include <filesystem>
#include <list>

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

	static FBXReader* _instance;

	std::string _path;
	std::filesystem::directory_iterator _directory;
	std::list<std::string> _fbxPaths;
};

