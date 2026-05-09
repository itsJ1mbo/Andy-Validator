#include "AndyValidator_Core/Application.h"

int main() 
{
	const std::unique_ptr<Application> app = std::make_unique<Application>();

	if (!app->init()) {
		return 1;
	}

	app->run();
	app->free();

    return 0;
}
