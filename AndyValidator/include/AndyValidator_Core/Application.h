#pragma once

#include <vector>
#include <memory>

#include "AndyValidator_FBX/Validation.h"

class Loader;

class Application
{
public:
	Application();
	~Application();

	bool init() const;
	void run();
	void free() const;

private:
	std::unique_ptr<Loader> _loader;

	std::vector<ValidationResults> _results;
};

