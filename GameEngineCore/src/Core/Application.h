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
        float maxDeltaTime = 0.04; // negative value for no max. Make freeze (and launch) not blow everything up and don't know if there's a better method
    };
public:
    Application(createInfo& createInfo);
    Application(const char * title, uint32_t width, uint32_t height, Engine::Scene* defaultScene, float maxDeltaTime);

    /*using RendererFactory = std::function<Engine::Renderer::Renderer*(Window&)>;*/
    /*Application(const char * title, uint32_t width, uint32_t height, Engine::Scene* defaultScene, RendererFactory rendererFactory);*/

    ~Application();

    void Run();
private:
    float m_maxDeltaTime = 0.0f;
     
    bool m_running = true;
    Window m_window;

    Engine::Renderer::Renderer* m_renderer;
    Engine::Scene* m_scene;
};

}

