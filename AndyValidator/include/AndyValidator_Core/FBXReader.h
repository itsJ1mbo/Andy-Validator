#pragma once

#include <string> 
#include <filesystem>
#include <list>
#include <memory>

class FBXReader
{
public:
	static FBXReader& instance();

	bool init() const;
	void readModels();

private:
	FBXReader() = default;
	FBXReader(const FBXReader& in) = delete;
	FBXReader& operator=(const FBXReader& in) = delete;
	FBXReader(const FBXReader&& in) = delete;
	FBXReader& operator=(const FBXReader&& in) = delete;

	void initDirectory();

	inline static std::unique_ptr<FBXReader> _instance;

	std::string _path;
	std::filesystem::directory_iterator _directory;
	std::list <std::filesystem::directory_entry> _fbxEntries;
};

