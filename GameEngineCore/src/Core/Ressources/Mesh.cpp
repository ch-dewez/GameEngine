#include "Mesh.h"

namespace Engine {
namespace Ressources {


/*Mesh::Mesh(std::string& path){*/
/**/
/*}*/

Mesh::Mesh(std::vector<uint32_t> indices, void* vertexData, size_t size)
{
    m_vertexBuffer = std::make_unique<Engine::Ressources::VertexBuffer>(vertexData, size);
    m_indexBuffer = std::make_unique<Engine::Ressources::IndexBuffer>(indices.data(), (size_t)indices.size() * sizeof(indices[0]), indices.size());
}

void Mesh::bind(Engine::Renderer::Renderer::FrameInfo frameInfo){
    auto& api = ::Engine::Renderer::VulkanApi::Instance();

    VkBuffer vertexBuffers[] = {m_vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    api.cmdBindVertexBuffers(frameInfo.commandBuffer, 0, 1, vertexBuffers, offsets);
    api.cmdBindIndexBuffer(frameInfo.commandBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);

};

void Mesh::draw(Engine::Renderer::Renderer::FrameInfo frameInfo){
    auto& api = ::Engine::Renderer::VulkanApi::Instance();
    api.cmdDrawIndexed(frameInfo.commandBuffer, m_indexBuffer->indexCount, 1, 0, 0, 0);
};

}
}
