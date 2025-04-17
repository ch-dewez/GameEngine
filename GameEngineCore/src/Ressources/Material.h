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
    MaterialTemplate(size_t matSize, std::shared_ptr<Pipeline> pipeline);
    /*MaterialTemplate(size_t bufferSize, std::string& pipelineKey);*/

    std::weak_ptr<Pipeline> getPipeline() {return m_pipeline;};
private:
    int allocateMaterialIndex();
    void updateData(void* data, int index);
    void deallocateMaterialIndex(int index);
    void bindDescriptorSet(Renderer::Renderer::FrameInfo frameInfo, int index);
private:
    static constexpr uint32_t MAX_MATERIALS = 100;  // Add a reasonable limit
    uint32_t m_nextMaterialIndex = 0;
    std::vector<uint32_t> m_freeIndices;  // Store available indices for reuse
    uint32_t m_sizeOfMaterial;
    std::shared_ptr<Pipeline> m_pipeline;
    VkDescriptorSetLayout m_matSetLayout;
    std::vector<VkDescriptorSet> m_matDescriptorSet;
    std::unique_ptr<UniformBuffer> m_matUniformBuffer;
};

class Material {
public:
    Material(std::shared_ptr<MaterialTemplate> matTemplate);
    ~Material();

    void updateData(void* data);
    void bind(Renderer::Renderer::FrameInfo frameInfo);
    std::weak_ptr<MaterialTemplate> getMaterialTemplate() {return m_matTemplate;};
private:
    std::shared_ptr<MaterialTemplate> m_matTemplate;
    int m_matIndex;
};

}
}
