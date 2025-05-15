#pragma once
#include "Scene/Scene.h"
#include "Renderer/Renderer.h"
#include "Window.h"
#include <GLFW/glfw3.h>
#include <cstdint>

namespace Engine {

class Application
{
public:
    struct createInfo {
        const char* title;
        uint32_t width;
        uint32_t height;
        Engine::Scene* defaultScene;
    };
public:
    Application(createInfo& createInfo);
    Application(const char * title, uint32_t width, uint32_t height, Engine::Scene* defaultScene);

    using RendererFactory = std::function<Engine::Renderer::Renderer*(Window&)>;
    Application(const char * title, uint32_t width, uint32_t height, Engine::Scene* defaultScene, RendererFactory rendererFactory);

    ~Application();

    void Run();
private:
    bool m_running = true;
    Window m_window;

    Engine::Renderer::Renderer* m_renderer;
    Engine::Scene* m_scene;
};

}

