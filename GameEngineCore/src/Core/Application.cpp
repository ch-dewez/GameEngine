#include "Application.h"
#include "Renderer/DefaultRenderer.h"
#include "Renderer/VulkanApi.h"
#include "Ressources/RessourceManager.h"
#include "Ressources/DescriptorsManager.h"
#include "Scene/Components/Renderer.h"
#include "Input.h"
#include <iostream>
#include "Collisions/Collisions.h"
#include "Log/Log.h"

namespace Engine {

Application::Application(createInfo& createInfo)
: Application(createInfo.title, createInfo.width, createInfo.height, createInfo.defaultScene, createInfo.maxDeltaTime)
{
};

Application::Application(const char* title, uint32_t width, uint32_t height, Engine::Scene* defaultScene, float maxDeltaTime)
: m_window(title, width, height), m_maxDeltaTime(maxDeltaTime)
{
    Engine::Renderer::VulkanApi::Init(m_window);
    Engine::Ressources::RessourceManager::Init();
    Engine::Ressources::DescriptorBuilder::Init();
    m_renderer = new Engine::Renderer::DefaultRenderer();
    
    // Set up the model buffer allocator and deallocator
    Engine::Components::Renderer::SetModelBufferAllocator([this]() {
        return static_cast<Engine::Renderer::DefaultRenderer*>(m_renderer)->allocateModelBufferIndex();
    });
    
    Engine::Components::Renderer::SetModelBufferDeallocator([this](uint32_t index) {
        static_cast<Engine::Renderer::DefaultRenderer*>(m_renderer)->deallocateModelBufferIndex(index);
    });
    
    m_scene = defaultScene;
    m_scene->initialize();
}

Application::~Application()
{
    delete m_scene;
    Engine::Ressources::DescriptorBuilder::DestroyAll();
    Engine::Ressources::RessourceManager::Shutdown();
    delete m_renderer;
    Engine::Renderer::VulkanApi::Shutdown();
}


void Application::Run()
{
    float lastTime = 0.0f;
    while (m_running && !m_window.shouldClose())
    {
        float currentTime = glfwGetTime();
        float dt = currentTime - lastTime;
        if (m_maxDeltaTime > 0) {
            dt = std::min(m_maxDeltaTime, dt);
        }

        lastTime = currentTime;

        m_scene->updateComponents(dt);

        Engine::Collisions::ManageCollision(*m_scene, dt);

        m_renderer->render(*m_scene);

        m_window.pollEvents();
        Engine::Input::Instance().Update();
    }

    Engine::Renderer::VulkanApi::Instance().deviceWaitIdle();
}


}

