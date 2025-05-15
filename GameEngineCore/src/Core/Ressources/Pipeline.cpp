#include "DescriptorsManager.h"
#include "Core/Renderer/VulkanApi.h"
#include "Pipeline.h"
#include "iostream"
#include "vulkan/vulkan_core.h"
#include <fstream>
#include <map>
#include <spirv_reflect.h>
#include <utility>
#include <vector>

namespace Engine {
namespace Ressources {

static std::pair<int, int> dynamicBuffers[] = {std::pair(1, 0)}; // set , binding

struct DescriptorSetLayoutData {
    std::map<uint32_t, VkDescriptorSetLayoutBinding> bindings;
    VkShaderStageFlags stageFlags;
};

std::map<uint32_t, VkDescriptorSetLayout> reflectShaderModule(
    const std::vector<std::pair<std::vector<char>, VkShaderStageFlags>>& shaderData) {
    std::map<uint32_t, DescriptorSetLayoutData> setLayouts;
    
    // Process each shader
    for (const auto& [spirvCode, shaderStage] : shaderData) {
        SpvReflectShaderModule module;
        SpvReflectResult result = spvReflectCreateShaderModule(
            spirvCode.size(),
            spirvCode.data(),
            &module);
        
        if (result != SPV_REFLECT_RESULT_SUCCESS) {
            spvReflectDestroyShaderModule(&module);
            throw std::runtime_error("Failed to create reflection module");
        }

        uint32_t count = 0;
        result = spvReflectEnumerateDescriptorSets(&module, &count, nullptr);
        std::vector<SpvReflectDescriptorSet*> sets(count);
        result = spvReflectEnumerateDescriptorSets(&module, &count, sets.data());

        // Process each set in the shader
        for (auto* set : sets) {
            auto setIndex = set->set;
            auto& setLayout = setLayouts[setIndex];

            // Process each binding in the set
            for (uint32_t b = 0; b < set->binding_count; b++) {
                const auto& binding = set->bindings[b];
                auto bindingIndex = binding->binding;


                // Check if this binding already exists
                auto& bindings = setLayout.bindings;
                if (bindings.find(bindingIndex) != bindings.end()) {
                    // Combine shader stages if binding already exists
                    bindings[bindingIndex].stageFlags |= shaderStage;
                } else {
                    // Create new binding
                    VkDescriptorSetLayoutBinding newBinding{};
                    newBinding.binding = bindingIndex;
                    newBinding.descriptorCount = 1;
                    newBinding.descriptorType = static_cast<VkDescriptorType>(binding->descriptor_type);
                    newBinding.stageFlags = shaderStage;
                    newBinding.pImmutableSamplers = nullptr;

                    // Check for dynamic buffer
                    for (const auto& [setIdx, bindingIdx] : dynamicBuffers) {
                        if (setIdx == setIndex && bindingIdx == bindingIndex) {
                            if (newBinding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                                newBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                            } else if (newBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
                                newBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
                            }
                            break;
                        }
                    }

                    bindings[bindingIndex] = newBinding;
                }
            }
        }

        spvReflectDestroyShaderModule(&module);
    }

    // Create final descriptor set layouts
    std::map<uint32_t, VkDescriptorSetLayout> finalLayouts;
    auto descriptorLayoutCache = DescriptorLayoutCache::Instance();

    for (const auto& [setIndex, layoutData] : setLayouts) {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        for (const auto& [_, binding] : layoutData.bindings) {
            bindings.push_back(binding);
        }

        VkDescriptorSetLayoutCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        createInfo.pBindings = bindings.data();

        finalLayouts[setIndex] = descriptorLayoutCache.create_descriptor_layout(&createInfo);
    }

    return finalLayouts;
}

PipelineConfigInfo PipelineConfigInfo::defaultPipelineConfigInfo(std::string vertShaderPath, const std::string fragShaderPath, VkVertexInputBindingDescription bindingDescription, std::vector<VkVertexInputAttributeDescription> attributeDescription) {
    PipelineConfigInfo configInfo{};

    configInfo.vertShaderPath = vertShaderPath;
    configInfo.fragShaderPath = fragShaderPath;

    //Vertex Info
    configInfo.bindingDescription = bindingDescription;
    configInfo.attributeDescriptions = attributeDescription;

    // Input Assembly
    configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    // Viewport & Scissor
    configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    configInfo.viewportInfo.viewportCount = 1;
    configInfo.viewportInfo.scissorCount = 1;

    // Rasterization
    configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
    configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    /*configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;*/
    configInfo.rasterizationInfo.lineWidth = 1.0f;
    configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    /*configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;*/
    configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
    configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;
    configInfo.rasterizationInfo.depthBiasClamp = 0.0f;
    configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;

    // Multisampling
    configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
    configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    configInfo.multisampleInfo.minSampleShading = 1.0f;
    configInfo.multisampleInfo.pSampleMask = nullptr;
    configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
    configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;

    // Depth and Stencil Testing
    configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.minDepthBounds = 0.0f;
    configInfo.depthStencilInfo.maxDepthBounds = 1.0f;
    configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.front = {};
    configInfo.depthStencilInfo.back = {};

    // Color Blending
    configInfo.colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
    configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
    configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    configInfo.colorBlendInfo.attachmentCount = 1;
    configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
    configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

    // Dynamic State
    configInfo.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
    configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
    configInfo.dynamicStateInfo.flags = 0;

    return configInfo;
}

Pipeline::Pipeline(PipelineConfigInfo configInfo)
: m_configInfo(configInfo) {
    createPipeline();
};

Pipeline::~Pipeline() {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    api.destroyPipeline(m_graphicsPipeline, nullptr);
    api.destroyPipelineLayout(m_pipelineLayout, nullptr);
}

static std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error(std::format("failed to open file! {}", filename));
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}


void Pipeline::createPipeline() {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();

    auto vertShaderCode = readFile(m_configInfo.vertShaderPath);
    auto fragShaderCode = readFile(m_configInfo.fragShaderPath);

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);


    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    // Vertex Input State
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_configInfo.attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &m_configInfo.bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = m_configInfo.attributeDescriptions.data();

    auto swapChainExtent = api.getSwapChainExtent();

    // Viewport State
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;

    m_configInfo.viewportInfo.pViewports = &viewport;
    m_configInfo.viewportInfo.pScissors = &scissor;

    // Pipeline Layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    // Get reflectance info from shaders
    std::vector<std::pair<std::vector<char>, VkShaderStageFlags>> shaderData = {
        {vertShaderCode, VK_SHADER_STAGE_VERTEX_BIT},
        {fragShaderCode, VK_SHADER_STAGE_FRAGMENT_BIT}
    };

    auto layouts = reflectShaderModule(shaderData);

    // Convert map to vector for pipeline layout creation
    std::vector<VkDescriptorSetLayout> layoutsVec;
    for (const auto& [setIndex, layout] : layouts) {
        if (layoutsVec.size() <= setIndex) {
            layoutsVec.resize(setIndex + 1, VK_NULL_HANDLE);
        }
        layoutsVec[setIndex] = layout;
    }

    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layoutsVec.size());
    pipelineLayoutInfo.pSetLayouts = layoutsVec.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (api.createPipelineLayout(&pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    };

    // Graphics Pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &m_configInfo.inputAssemblyInfo;
    pipelineInfo.pViewportState = &m_configInfo.viewportInfo;
    pipelineInfo.pRasterizationState = &m_configInfo.rasterizationInfo;
    pipelineInfo.pMultisampleState = &m_configInfo.multisampleInfo;
    pipelineInfo.pDepthStencilState = &m_configInfo.depthStencilInfo;
    pipelineInfo.pColorBlendState = &m_configInfo.colorBlendInfo;
    pipelineInfo.pDynamicState = &m_configInfo.dynamicStateInfo;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = api.getRenderPass();
    pipelineInfo.subpass = m_configInfo.subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if (api.createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    // Cleanup
    api.destroyShaderModule(fragShaderModule, nullptr);
    api.destroyShaderModule(vertShaderModule, nullptr);
}

VkShaderModule Pipeline::createShaderModule(const std::vector<char> &code) {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (api.createShaderModule(&createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void Pipeline::recreatePipeline() {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    // Clean up old pipeline
    api.destroyPipeline(m_graphicsPipeline, nullptr);

    // Update render pass in config
    m_configInfo.renderPass = api.getRenderPass();

    // Recreate pipeline with stored config
    createPipeline();
}

} // namespace Assets
} // namespace Engine
