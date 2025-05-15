#pragma once
#include "Core/Renderer/Renderer.h"
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
    MaterialTemplate(std::unique_ptr<Pipeline> pipeline);
    /*MaterialTemplate(size_t bufferSize, std::string& pipelineKey);*/

    Pipeline* getPipeline() {return m_pipeline.get();};
    void bindPipeline(Renderer::Renderer::FrameInfo frameInfo);
private:
    std::unique_ptr<Pipeline> m_pipeline;
};

class Material {
public:
    Material(std::shared_ptr<MaterialTemplate> matTemplate, size_t matSize);
    Material(std::shared_ptr<MaterialTemplate> matTemplate, size_t matSize, std::vector<VkDescriptorImageInfo>* texturesInfo);

    void updateData(void* data);
    void bindDescriptorSet(Renderer::Renderer::FrameInfo frameInfo);
    void bind(Renderer::Renderer::FrameInfo frameInfo); // should not be called but it's there (pipeline is already bind in the renderer)
    MaterialTemplate* getMaterialTemplate() {return m_matTemplate.get();}; // I don't want to deal with weak_ptr this func is just to get the pipeline
private:
    std::shared_ptr<MaterialTemplate> m_matTemplate;
    uint32_t m_sizeOfMaterial;
    std::vector<VkDescriptorSet> m_matDescriptorSet;
    std::unique_ptr<UniformBuffer> m_matUniformBuffer;
};

}
}
