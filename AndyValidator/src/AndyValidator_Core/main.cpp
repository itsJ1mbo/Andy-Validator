#include <iostream>
#include "AndyValidator_Core/FBXReader.h"
#include "AndyValidator_Window/Window.h"

int main() {
    
    //if (!FBXReader::instance().init()) {
    //    return 1;
    //}
    if (!Window::instance().init()) {
        return 1;
    }

    //FBXReader::instance().readModels();

    while (!Window::instance().shouldWindowClose()) {
        Window::instance().updateWindow();
    }

    Window::instance().free();

    return 0;
}