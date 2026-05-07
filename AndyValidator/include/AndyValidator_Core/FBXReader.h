#pragma once

#include <string> 
#include <filesystem>
#include <list>

class FBXReader
{
public:
	FBXReader();
	void readModels();

	std::list<std::string> getModelPaths() const
	{
		std::list<std::string> paths;
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
	std::list <std::filesystem::directory_entry> _fbxEntries;
};

