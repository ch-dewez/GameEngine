#include "MeshRenderer.h"
#include "Renderer/VulkanApi.h"
#include "Scene/Entities/Entity.h"
#include "Scene/Components/Transform.h"
#include "Ressources/DescriptorsManager.h"
#include "Renderer/DefaultRenderer.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <memory>

namespace Engine {
namespace Components {

MeshRenderer::MeshRenderer(std::shared_ptr<Ressources::Material> material, std::unique_ptr<Ressources::VertexBuffer> vertexBuffer, std::unique_ptr<Ressources::IndexBuffer> indexBuffer)
: Renderer(material), m_vertexBuffer(std::move(vertexBuffer)), m_indexBuffer(std::move(indexBuffer))
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
    VkBuffer vertexBuffers[] = {m_vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    api.cmdBindVertexBuffers(frameInfo.commandBuffer, 0, 1, vertexBuffers, offsets);
    api.cmdBindIndexBuffer(frameInfo.commandBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);

    api.cmdDrawIndexed(frameInfo.commandBuffer, m_indexBuffer->indexCount, 1, 0, 0, 0);
}


}
}
