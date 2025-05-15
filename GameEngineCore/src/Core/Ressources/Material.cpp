#include "Material.h"
#include "Core/Renderer/VulkanApi.h"
#include "Core/Renderer/Renderer.h"
#include "Pipeline.h"
#include "Material.h"
#include "DescriptorsManager.h"
#include "Core/Renderer/VulkanApi.h"
#include "UniformBuffer.h"
#include <memory>
#include <vector>

namespace Engine {
namespace Ressources {

MaterialTemplate::MaterialTemplate(std::unique_ptr<Pipeline> pipeline)
{
    m_pipeline = std::move(pipeline);
};

void MaterialTemplate::bindPipeline(Renderer::Renderer::FrameInfo frameInfo){
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    api.cmdBindPipeline(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getPipeline());
};

Material::Material(std::shared_ptr<MaterialTemplate> matTemplate, size_t matSize)
: Material(matTemplate, matSize, nullptr)
{
}

Material::Material(std::shared_ptr<MaterialTemplate> matTemplate, size_t matSize, std::vector<VkDescriptorImageInfo>* texturesInfo)
: m_matTemplate(matTemplate), m_sizeOfMaterial(matSize)
{
    m_matUniformBuffer = std::make_unique<UniformBuffer>(m_sizeOfMaterial, 1);

    m_matDescriptorSet.resize(1);

    for (size_t i = 0;i < 1; i++) {
        VkDescriptorBufferInfo matBufferInfo{};
        matBufferInfo.buffer = m_matUniformBuffer->getBuffer(i);
        matBufferInfo.offset = 0;
        matBufferInfo.range = sizeof(m_sizeOfMaterial); // Size of one model matrix

        auto descriptorBuilder = ::Engine::Ressources::DescriptorBuilder();
        descriptorBuilder
            .bind_buffer(0, &matBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
        if (texturesInfo) {
            int binding = 1;
            for (VkDescriptorImageInfo textureInfo : *texturesInfo) {
                descriptorBuilder.bind_image(binding, &textureInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
                binding ++;
            }
        }
        m_matDescriptorSet[i] = descriptorBuilder.build();
    }

};

void Material::updateData(void* data) {
    m_matUniformBuffer->updateData(data, m_sizeOfMaterial);
}

void Material::bindDescriptorSet(Renderer::Renderer::FrameInfo frameInfo) {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    api.cmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_matTemplate->getPipeline()->getPipelineLayout(), 2, 1, &m_matDescriptorSet[0], 0, nullptr);
}

void Material::bind(Renderer::Renderer::FrameInfo frameInfo) {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    m_matTemplate->bindPipeline(frameInfo);
    api.cmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_matTemplate->getPipeline()->getPipelineLayout(), 2, 1, &m_matDescriptorSet[0], 0, nullptr);
}

}
}
