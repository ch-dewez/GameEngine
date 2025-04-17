#pragma once
#include <vector>
#include <GLFW/glfw3.h>

static std::vector<char> readFile(const std::string& filename);

namespace Engine {
namespace Ressources {


struct PipelineConfigInfo {
    std::string vertShaderPath;
    std::string fragShaderPath;
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;

    static PipelineConfigInfo defaultPipelineConfigInfo(std::string vertShaderPath, const std::string fragShaderPath);
};

class Pipeline {
public:
    Pipeline(PipelineConfigInfo configInfo);

    ~Pipeline();

    VkPipeline& getPipeline() { return m_graphicsPipeline; };
    VkPipelineLayout& getPipelineLayout() { return m_pipelineLayout; };

    void createPipeline();
    void recreatePipeline();

    VkShaderModule createShaderModule(const std::vector<char>& code);

private:
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    PipelineConfigInfo m_configInfo;
};

}
}
