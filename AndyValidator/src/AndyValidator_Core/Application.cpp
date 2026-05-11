#include "AndyValidator_Core/Application.h"
#include "AndyValidator_Core/Loader.h"
#include "AndyValidator_FBX/FBX.h"
#include "AndyValidator_Window/Window.h"

#include <iostream>

Application::Application() : _loader(std::make_unique<Loader>()), _results(_loader->getModelPaths().size())
{

}

Application::~Application() = default;

bool Application::init() const
{
    if (!FBX::instance().init()) {
        return false;
    }
    if (!Window::instance().init()) {
        return false;
    }

    return true;
}

void Application::run()
{
    FBX::instance().start(_loader->getModelPaths(), _loader->getConfig());

    while (!Window::instance().shouldWindowClose())
    {
        const auto results = FBX::instance().checkNewResults();
        for (const auto& result : results)
    	{
            _results[result.index] = result;
        }

        Window::instance().updateWindow();
    }
}

void Application::free() const
{
    Window::instance().free();
    FBX::instance().free();
}
