#pragma  once
#include "Renderer.h"
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include "Core/Ressources/UniformBuffer.h"


namespace Engine {
namespace Renderer {


class DefaultRenderer: public Renderer {
public:
    struct GlobalUniformBufferObject {
        glm::mat4 view;
        glm::mat4 proj;
    };

    static constexpr uint32_t MAX_OBJECTS = 1000;

    DefaultRenderer();

    void render(Engine::Scene& scene) override;
    uint32_t allocateModelBufferIndex(); // Returns next available index
    void deallocateModelBufferIndex(uint32_t index); // Deallocates the given index

    VkDescriptorSetLayout globalDescriptorLayout;
    VkDescriptorSetLayout modelDescriptorLayout;
private:
    std::vector<VkDescriptorSet> m_globalDescriptorSets;
    std::unique_ptr<Engine::Ressources::UniformBuffer> m_globalUniformBuffer;
    std::unique_ptr<Engine::Ressources::UniformBuffer> m_lightsUniformBuffer;
    std::vector<VkDescriptorSet> m_modelDescriptorSets;
    std::unique_ptr<Engine::Ressources::UniformBuffer> m_modelUniformBuffer;
    uint32_t m_nextModelBufferIndex = 0;
    std::vector<uint32_t> m_freeIndices; // Store available indices for reuse
};

}
}
