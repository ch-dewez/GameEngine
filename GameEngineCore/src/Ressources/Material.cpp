#include "Material.h"
#include "Renderer/VulkanApi.h"
#include "Renderer/Renderer.h"
#include "Pipeline.h"
#include "Material.h"
#include "DescriptorsManager.h"
#include "Renderer/VulkanApi.h"
#include "UniformBuffer.h"
#include <memory>

namespace Engine {
namespace Ressources {

MaterialTemplate::MaterialTemplate(size_t matSize, std::shared_ptr<Pipeline> pipeline)
: m_pipeline(pipeline), m_sizeOfMaterial(matSize), m_matSetLayout()
{
    size_t bufferSize= matSize * MAX_MATERIALS;
    m_matUniformBuffer = std::make_unique<UniformBuffer>(bufferSize, 1);

    m_matDescriptorSet.resize(1);

    for (size_t i = 0;i < 1; i++) {
        VkDescriptorBufferInfo matBufferInfo{};
        matBufferInfo.buffer = m_matUniformBuffer->getBuffer(i);
        matBufferInfo.offset = 0;
        matBufferInfo.range = sizeof(bufferSize); // Size of one model matrix

        auto descriptorBuilder = ::Engine::Ressources::DescriptorBuilder();
        descriptorBuilder
            .bind_buffer(0, &matBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_FRAGMENT_BIT);
        m_matDescriptorSet[i] = descriptorBuilder.build(m_matSetLayout);
    }
};

/*MaterialTemplate::MaterialTemplate(size_t bufferSize, std::string& pipelineKey) {*/
/**/
/*}*/


int MaterialTemplate::allocateMaterialIndex() {
    // If we have free indices available, use one of those
    if (!m_freeIndices.empty()) {
        int index = m_freeIndices.back();
        m_freeIndices.pop_back();
        return index;
    }

    // If no free indices and we haven't reached the limit, create a new one
    if (m_nextMaterialIndex >= MAX_MATERIALS) {
        throw std::runtime_error("Exceeded maximum number of materials");
    }
    
    return m_nextMaterialIndex++;
}

void MaterialTemplate::deallocateMaterialIndex(int index) {
    // Validate the index
    if (index >= m_nextMaterialIndex) {
        throw std::runtime_error("Invalid material index deallocation");
    }

    // Check if this index is already in the free list
    if (std::find(m_freeIndices.begin(), m_freeIndices.end(), index) != m_freeIndices.end()) {
        throw std::runtime_error("Attempting to deallocate already freed material index");
    }

    // Add the index to the free list for reuse
    m_freeIndices.push_back(index);
}

void MaterialTemplate::updateData(void* data, int index) {
    size_t offset = index * m_sizeOfMaterial;
    m_matUniformBuffer->updateData(data, m_sizeOfMaterial, 0, offset);
}

void MaterialTemplate::bindDescriptorSet(Renderer::Renderer::FrameInfo frameInfo, int index){
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    api.cmdBindPipeline(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getPipeline());
    uint32_t dynamicOffset = m_sizeOfMaterial * index;
    api.cmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getPipelineLayout(), 2, 1, &m_matDescriptorSet[0], 1, &dynamicOffset);
};

Material::Material(std::shared_ptr<MaterialTemplate> matTemplate)
: m_matTemplate(matTemplate)
{
    m_matIndex = m_matTemplate->allocateMaterialIndex();
}

Material::~Material() {
    m_matTemplate->deallocateMaterialIndex(m_matIndex);
}

void Material::updateData(void* data) {
    m_matTemplate->updateData(data, m_matIndex);
}

void Material::bind(Renderer::Renderer::FrameInfo frameInfo) {
    m_matTemplate->bindDescriptorSet(frameInfo, m_matIndex);
}

}
}
