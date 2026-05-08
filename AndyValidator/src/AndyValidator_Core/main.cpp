#include "AndyValidator_Core/Loader.h"
#include "AndyValidator_Window/Window.h"
#include "AndyValidator_FBX/FBX.h"

#include <memory>

int main() 
{
    if (!FBX::instance().init()) {
        return 1;
    }
    if (!Window::instance().init()) {
        return 1;
    }

    const std::unique_ptr<Loader> loader = std::make_unique<Loader>();

    loader->readModels();

    while (!Window::instance().shouldWindowClose()) {
        Window::instance().updateWindow();
    }

    Window::instance().free();
    FBX::instance().free();

    return 0;
}