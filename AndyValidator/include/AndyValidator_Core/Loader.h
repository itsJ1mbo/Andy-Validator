#pragma once

#include <string> 
#include <filesystem>
#include <vector>

#include "ModelData.h"

class Loader
{
public:
	Loader();

	std::vector<std::string> getModelPaths() const
	{
		return _fbxEntries;
	}
	Config getConfig() const
	{
		return _cfg;
	}

private:
	void initDirectory();

	void readConfig(const std::string& path);
	void createConfig();
	void read();

	std::string _path;
	std::filesystem::directory_iterator _directory;
	
	std::vector<std::string> _fbxEntries;
	Config _cfg;
	bool _configRead = false;
};

