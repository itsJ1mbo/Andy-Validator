#include "AndyValidator_Core/Application.h"
#if !_DEBUG
#include <windows.h>
#endif

#if _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	const std::unique_ptr<Application> app = std::make_unique<Application>();

	if (!app->init()) {
		return 1;
	}

	app->run();
	app->free();

    return 0;
}
