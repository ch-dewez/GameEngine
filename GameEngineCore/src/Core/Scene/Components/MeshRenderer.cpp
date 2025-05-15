#include "MeshRenderer.h"
#include "Core/Renderer/VulkanApi.h"
#include "Core/Scene/Entities/Entity.h"
#include "Core/Scene/Components/Transform.h"
#include "Core/Ressources/DescriptorsManager.h"
#include "Core/Renderer/DefaultRenderer.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <memory>

namespace Engine {
namespace Components {

MeshRenderer::MeshRenderer(std::shared_ptr<Ressources::Material> material, std::shared_ptr<Ressources::Mesh> mesh)
: Renderer(material), m_mesh(mesh)
{
}



void MeshRenderer::render(Engine::Renderer::Renderer::FrameInfo& frameInfo) {
    auto& api = ::Engine::Renderer::VulkanApi::Instance();

    // Update and bind the model matrix
    glm::mat4 model = m_entity->getComponent<Transform>().value().lock()->getModelMatrix();
    uint32_t offset = m_modelBufferIndex * sizeof(glm::mat4);
    frameInfo.modelsBuffer->updateData(&model, sizeof(glm::mat4), frameInfo.frameIndex, offset);

    // Bind the model descriptor set
    api.cmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_material->getMaterialTemplate()->getPipeline()->getPipelineLayout(), 1, 1, &frameInfo.modelsSet, 1, &offset);

    m_material->bindDescriptorSet(frameInfo);

    // Bind vertex and index buffers
    m_mesh->bind(frameInfo);
    m_mesh->draw(frameInfo);
}


}
}
