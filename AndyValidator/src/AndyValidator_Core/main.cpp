#include "AndyValidator_Core/FBXReader.h"
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

    const std::unique_ptr<FBXReader> fbxReader = std::make_unique<FBXReader>();

    fbxReader->readModels();
    if (!FBX::instance().import(fbxReader->getModelPaths())) {
        return 1;
    }

    while (!Window::instance().shouldWindowClose()) {
        Window::instance().updateWindow();
    }

    Window::instance().free();
    FBX::instance().free();

    return 0;
}