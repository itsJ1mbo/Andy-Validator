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
		std::vector<std::string> paths;
		for (const auto& entry : _fbxEntries)
		{
			paths.push_back(entry.path().string());
		}
		return paths;
	}

private:
	void initDirectory();

	std::string _path;
	std::filesystem::directory_iterator _directory;
	std::vector<std::filesystem::directory_entry> _fbxEntries;
};

