#include "EntryPoint.h"
#include "Application.h"
#include <iostream>
#include "Log/Log.h"

extern Engine::Application::createInfo CreateAppInfo();

int main(int argc, char** argv)
{
    Engine::Log::Log::init(); 
    auto info = CreateAppInfo();
    Engine::Application app(info);
    try
    {
        app.Run();
    }
    catch (std::exception e)
    {
        std::cout << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
