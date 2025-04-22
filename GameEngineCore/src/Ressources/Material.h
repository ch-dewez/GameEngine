#pragma once
#include "Renderer/Renderer.h"
#include "Pipeline.h"
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include "UniformBuffer.h"

namespace Engine {
namespace Ressources {
//forward
class Material;

class MaterialTemplate {
// don't want non mat allocating things here
friend Material;
public:
    MaterialTemplate(std::shared_ptr<Pipeline> pipeline);
    /*MaterialTemplate(size_t bufferSize, std::string& pipelineKey);*/

    std::weak_ptr<Pipeline> getPipeline() {return m_pipeline;};
    void bindPipeline(Renderer::Renderer::FrameInfo frameInfo);
private:
    std::shared_ptr<Pipeline> m_pipeline;
};

class Material {
public:
    Material(std::shared_ptr<MaterialTemplate> matTemplate, size_t matSize);
    Material(std::shared_ptr<MaterialTemplate> matTemplate, size_t matSize, std::vector<VkDescriptorImageInfo>* texturesInfo);
    ~Material();

    void updateData(void* data);
    void bind(Renderer::Renderer::FrameInfo frameInfo);
    std::weak_ptr<MaterialTemplate> getMaterialTemplate() {return m_matTemplate;};
private:
    std::shared_ptr<MaterialTemplate> m_matTemplate;
    uint32_t m_sizeOfMaterial;
    VkDescriptorSetLayout m_matSetLayout;
    std::vector<VkDescriptorSet> m_matDescriptorSet;
    std::unique_ptr<UniformBuffer> m_matUniformBuffer;
};

}
}
