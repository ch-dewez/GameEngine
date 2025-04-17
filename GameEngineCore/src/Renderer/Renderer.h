#pragma once
#include <cstdint>
#include <vector>
#include "Scene/Scene.h"
#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>
#include "Ressources/UniformBuffer.h"

namespace Engine {
namespace Renderer {

class Renderer {
public:
    struct FrameInfo {
        int frameIndex = 0;
        VkCommandBuffer commandBuffer;
        VkDescriptorSet globalSet;
        VkDescriptorSet modelsSet;
        Ressources::UniformBuffer* modelsBuffer = nullptr;
    };

    Renderer();
    virtual ~Renderer();

    void beginFrame();
    void endFrame();

    virtual void render(Engine::Scene& scene) = 0;
    
    void beginRenderPass();
    void endRenderPass();

protected:

    void createSyncObjects();
    void createCommandBuffers();
protected:
    FrameInfo m_frameInfo;

    std::vector<VkCommandBuffer> m_commandBuffers;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;

    uint32_t m_currentFrame = 0;

    uint32_t m_currentImageIndex = 0;

};


}
}
