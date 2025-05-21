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
    app.Run();

    return EXIT_SUCCESS;
}
