#pragma once

#include <string> 
#include <filesystem>
#include <vector>

class Loader
{
public:
	Loader();

	void readModels();

	std::vector<std::string> getModelPaths() const
	{
		return _fbxEntries;
	}

private:
	void initDirectory();

	std::string _path;
	std::filesystem::directory_iterator _directory;
	std::vector<std::string> _fbxEntries;
};

