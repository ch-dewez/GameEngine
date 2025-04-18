#include "Application.h"
#include "Renderer/DefaultRenderer.h"
#include "Renderer/VulkanApi.h"
#include "Ressources/RessourceManager.h"
#include "Ressources/DescriptorsManager.h"
#include "Scene/Components/Renderer.h"
#include "Input.h"

Application::Application(char* title, uint32_t width, uint32_t height, Engine::Scene* defaultScene)
: m_window(title, width, height)
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

Application::Application(char* title, uint32_t width, uint32_t height, Engine::Scene* defaultScene, RendererFactory rendererFactory)
: m_window(title, width, height)
{
    m_renderer = rendererFactory(m_window);
    
    m_scene = defaultScene;
    m_scene->initialize();
}

Application::~Application()
{
    delete m_scene;
    Engine::Ressources::RessourceManager::Shutdown();
    Engine::Ressources::DescriptorBuilder::DestroyAll();
    delete m_renderer;
    Engine::Renderer::VulkanApi::Shutdown();
}


void Application::Run()
{
    while (m_running && !m_window.shouldClose())
    {
        m_scene->updateComponents();

        m_renderer->render(*m_scene);

        m_window.pollEvents();
        Engine::Input::Instance().Update();
    }

    Engine::Renderer::VulkanApi::Instance().deviceWaitIdle();
}

