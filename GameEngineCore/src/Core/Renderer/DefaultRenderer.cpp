#include "DefaultRenderer.h"
#include "Lights.h"
#include "VulkanApi.h"
#include <cstring>
#include <map>
#include <memory>
#include "Core/Ressources/DescriptorsManager.h"
#include "Core/Ressources/UniformBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Scene/Entities/Entity.h"
#include "Core/Scene/Components/Renderer.h"
//TODO: remove this
#include "Core/Scene/Components/MeshRenderer.h"
#include "Core/Scene/Components/Camera.h"
#include "Core/Renderer/Lights.h"
#include "Core/Scene/Components/PointLight.h"
#include "Core/Scene/Components/DirectionalLight.h"
#include "vulkan/vulkan_core.h"

namespace Engine {
namespace Renderer {

DefaultRenderer::DefaultRenderer()
    : Renderer()
{
    auto maxFramesInFlight = VulkanApi::Instance().getMaxFramesInFlight();
    m_globalDescriptorSets.resize(maxFramesInFlight);
    m_modelDescriptorSets.resize(maxFramesInFlight);

    m_globalUniformBuffer = std::make_unique<Ressources::UniformBuffer>(sizeof(GlobalUniformBufferObject), maxFramesInFlight);
    m_modelUniformBuffer = std::make_unique<Ressources::UniformBuffer>(sizeof(glm::mat4) * MAX_OBJECTS, maxFramesInFlight);
    m_lightsUniformBuffer = std::make_unique<Ressources::UniformBuffer>(sizeof(LightEnvironment), maxFramesInFlight);

    // Set up global descriptor sets
    for (size_t i = 0; i < maxFramesInFlight; i++) {
        VkDescriptorBufferInfo globalBufferInfo{};
        globalBufferInfo.buffer = m_globalUniformBuffer->getBuffer(i);
        globalBufferInfo.offset = 0;
        globalBufferInfo.range = sizeof(GlobalUniformBufferObject);

        VkDescriptorBufferInfo lightsBufferInfo{};
        lightsBufferInfo.buffer = m_lightsUniformBuffer->getBuffer(i);
        lightsBufferInfo.offset = 0;
        lightsBufferInfo.range = sizeof(LightEnvironment); // Size of one model matrix

        auto descriptorBuilder = Engine::Ressources::DescriptorBuilder();
        descriptorBuilder
            .bind_buffer(0, &globalBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
            .bind_buffer(1, &lightsBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
        /*m_globalDescriptorSets[i] = descriptorBuilder.build(&globalDescriptorLayout);*/
        m_globalDescriptorSets[i] = descriptorBuilder.build();

        // Set up model descriptor sets with dynamic uniform buffer
        VkDescriptorBufferInfo modelBufferInfo{};
        modelBufferInfo.buffer = m_modelUniformBuffer->getBuffer(i);
        modelBufferInfo.offset = 0;
        modelBufferInfo.range = sizeof(glm::mat4); // Size of one model matrix

        descriptorBuilder = Engine::Ressources::DescriptorBuilder();
        descriptorBuilder
            .bind_buffer(0, &modelBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT);
        /*m_modelDescriptorSets[i] = descriptorBuilder.build(&modelDescriptorLayout);*/
        m_modelDescriptorSets[i] = descriptorBuilder.build();
    }
}

uint32_t DefaultRenderer::allocateModelBufferIndex() {
    // If we have free indices available, use one of those
    if (!m_freeIndices.empty()) {
        uint32_t index = m_freeIndices.back();
        m_freeIndices.pop_back();
        return index;
    }

    // If no free indices and we haven't reached the limit, create a new one
    if (m_nextModelBufferIndex >= MAX_OBJECTS) {
        throw std::runtime_error("Exceeded maximum number of objects");
    }
    
    return m_nextModelBufferIndex++;
}

void DefaultRenderer::deallocateModelBufferIndex(uint32_t index) {
    // Validate the index
    if (index >= m_nextModelBufferIndex) {
        throw std::runtime_error("Invalid model buffer index deallocation");
    }

    // Check if this index is already in the free list
    if (std::find(m_freeIndices.begin(), m_freeIndices.end(), index) != m_freeIndices.end()) {
        throw std::runtime_error("Attempting to deallocate already freed index");
    }

    // Add the index to the free list for reuse
    m_freeIndices.push_back(index);
}

void DefaultRenderer::render(Engine::Scene& scene) {
    beginFrame();
    beginRenderPass();

    GlobalUniformBufferObject ubo{};
    {
        Engine::Components::Camera* camera = scene.getEntityByTag("Main Camera").value()->getComponent<Engine::Components::Camera>().value();
        ubo.view = camera->getViewMatrix();
        auto swapChainExtent = VulkanApi::Instance().getSwapChainExtent();
        ubo.proj = camera->getProjectionMatrix(swapChainExtent.width / (float)swapChainExtent.height);
    }

    m_globalUniformBuffer->updateData(&ubo, sizeof(GlobalUniformBufferObject), m_currentFrame);
    m_frameInfo.globalSet = m_globalDescriptorSets[m_currentFrame];

    // Get the pipeline layout from the first renderer component we find
    // since all pipelines should share the same global descriptor set layout
    VkPipelineLayout* pipelineLayout = nullptr;
    auto rendererComponents = scene.getComponentsRigistry().getAllElementOfType<Components::Renderer>();
    if (rendererComponents.size() <= 0){
        endRenderPass();
        endFrame();
        return;
    }
    pipelineLayout = &rendererComponents[0]->getMaterial().lock()->getMaterialTemplate()->getPipeline()->getPipelineLayout();


    // get all the lights
    LightEnvironment lightEnvironment;
    int pointLightIndex = 0;
    int directionalLightIndex = 0;

    auto& pointLights = scene.getComponents<Components::PointLight>();
    for (auto& pointLight : pointLights){
        lightEnvironment.pointLights[pointLightIndex] = pointLight.lightInfo;
        pointLightIndex ++;
    }
    
    auto& dirLights = scene.getComponents<Components::DirectionalLight>();
    for (auto& dirLight : dirLights){
        lightEnvironment.directionalLights[directionalLightIndex] = dirLight.lightInfo;
        directionalLightIndex ++;
    }

    lightEnvironment.nbDirectionalLight = directionalLightIndex;
    lightEnvironment.nbPointLight = pointLightIndex;

    m_lightsUniformBuffer->updateData(&lightEnvironment, sizeof(LightEnvironment), m_frameInfo.frameIndex);

    VulkanApi& api = VulkanApi::Instance();
    if (pipelineLayout != VK_NULL_HANDLE) {
        api.cmdBindDescriptorSets(
            m_frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            *pipelineLayout,
            0,
            1,
            &m_frameInfo.globalSet,
            0,
            nullptr
        );
    }

    // Store model descriptor set in frameInfo for use by renderers
    m_frameInfo.modelsSet = m_modelDescriptorSets[m_currentFrame];
    m_frameInfo.modelsBuffer = m_modelUniformBuffer.get();

    // Group renderers by material template
    std::map<Engine::Ressources::MaterialTemplate*, std::vector<Engine::Components::Renderer*>> renderGroups;
    
    for (Components::Renderer* component : rendererComponents) {
        auto material = component->getMaterial().lock();
        auto materialTemplate = material->getMaterialTemplate();
        renderGroups[materialTemplate].push_back(component);
    }
    
    // Render each group
    for (const auto& group : renderGroups) {
        group.first->bindPipeline(m_frameInfo);
        for (const auto& component : group.second) {
            component->render(m_frameInfo);
        }
    }

    endRenderPass();
    endFrame();
}

}
}
