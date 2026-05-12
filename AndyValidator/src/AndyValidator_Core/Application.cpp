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

    Window::instance().setModelNames(_loader->getModelPaths());

    while (!Window::instance().shouldWindowClose())
    {
        const auto results = FBX::instance().checkNewResults();
        for (const auto& result : results)
    	{
            _results[*result.index] = result;
            //std::cout << "\n[Test1] Resultado: " << (result.test1 ? "Aprobado" : "Suspenso");
            //std::cout << "\n[Polygon] Resultado: " << (result.polygon ? "Aprobado" : "Suspenso");
            //std::cout << "\n[3V Faces] Resultado: " << (result.validFaces ? "Aprobado" : "Suspenso");
            //std::cout << "\n[Normals] Resultado: " << (result.normals ? "Aprobado" : "Suspenso");
            //std::cout << "\n[PruebaX] Resultado: " << (result.pruebaX ? "Aprobado" : "Suspenso");
            //std::cout << "\n[PruebaY] Resultado: " << (result.pruebaY ? "Aprobado" : "Suspenso");
            //std::cout << "\n";
            //for (int i = 0; i < 50; i++) std::cout << "=";
            //std::cout << "\n";
        }        
        Window::instance().updateWindow(_results);
    }
}

void Application::free() const
{
    Window::instance().free();
    FBX::instance().free();
}
