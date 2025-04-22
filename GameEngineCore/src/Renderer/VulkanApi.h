#pragma once
#include "Window.h"
#include <GLFW/glfw3.h>

namespace Engine {

namespace Ressources {
//forward declaration
class Texture;
}

namespace Renderer {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();;
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanApi {
public:
    VulkanApi(Window& window);
    ~VulkanApi();

    static VulkanApi& Instance();
    static void Init(Window& window);
    static void Shutdown();

    VkDevice& getDevice() { return m_device; };
    VkPhysicalDevice& getPhysicalDevice() { return m_physicalDevice; };
    VkRenderPass& getRenderPass() { return m_renderPass; };
    VkExtent2D& getSwapChainExtent() { return m_swapChainExtent; };
    VkCommandPool& getCommandPool(){ return m_commandPool; };
    VkQueue& getGraphicsQueue() {return m_graphicsQueue; };
    VkSwapchainKHR& getSwapChain() {return m_swapChain; };
    VkFramebuffer& getSwapChainFrameBuffer(int index) { return m_swapChainFramebuffers[index]; };
    bool frameBufferResized() { return m_framebufferResized; };
    void setFrameBufferResized(bool value) {m_framebufferResized = value; };

    void deviceWaitIdle();

    void addRecreationCallback(std::function<void(VkRenderPass&, VkExtent2D&)> callback) {
        m_recreationCallbacks.push_back(callback);
    }


    int getMaxFramesInFlight() {return MAX_FRAMES_IN_FLIGHT; };

    // Buffer operations
    VkResult createBuffer(
        const VkBufferCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkBuffer* pBuffer);
    
    void destroyBuffer(
        VkBuffer buffer,
        const VkAllocationCallbacks* pAllocator);

    // Memory operations
    VkResult allocateMemory(
        const VkMemoryAllocateInfo* pAllocateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDeviceMemory* pMemory);

    void freeMemory(
        VkDeviceMemory memory,
        const VkAllocationCallbacks* pAllocator);

    void unmapMemory(VkDeviceMemory memory);
    
    void getBufferMemoryRequirements(
        VkBuffer buffer,
        VkMemoryRequirements* pMemoryRequirements);
        
    VkResult bindBufferMemory(
        VkBuffer buffer,
        VkDeviceMemory memory,
        VkDeviceSize memoryOffset);
        
    void getPhysicalDeviceMemoryProperties(
        VkPhysicalDeviceMemoryProperties* pMemoryProperties);
        
    VkResult mapMemory(
        VkDeviceMemory memory,
        VkDeviceSize offset,
        VkDeviceSize size,
        VkMemoryMapFlags flags,
        void** ppData);

    // Command buffer operations
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    VkResult allocateCommandBuffers(
        const VkCommandBufferAllocateInfo* pAllocateInfo,
        VkCommandBuffer* pCommandBuffers);

    void freeCommandBuffers(
        uint32_t commandBufferCount,
        const VkCommandBuffer* pCommandBuffers);

    // Image operations
    VkResult createImage(
        const VkImageCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkImage* pImage);

    VkResult createImageView(
        const VkImageViewCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkImageView* pView);

    void destroyImageView(
        VkImageView imageView,
        const VkAllocationCallbacks* pAllocator);
        
    void destroyImage(
        VkImage image,
        const VkAllocationCallbacks* pAllocator);
        
    void getImageMemoryRequirements(
        VkImage image,
        VkMemoryRequirements* pMemoryRequirements);
        
    VkResult bindImageMemory(
        VkImage image,
        VkDeviceMemory memory,
        VkDeviceSize memoryOffset);

    // Command Buffer Recording
    VkResult beginCommandBuffer(
        VkCommandBuffer commandBuffer,
        const VkCommandBufferBeginInfo* pBeginInfo);
    
    VkResult endCommandBuffer(VkCommandBuffer commandBuffer);
    
    void cmdBeginRenderPass(
        VkCommandBuffer commandBuffer,
        const VkRenderPassBeginInfo* pRenderPassBegin,
        VkSubpassContents contents);
        
    void cmdEndRenderPass(VkCommandBuffer commandBuffer);
    
    void cmdBindPipeline(
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipeline pipeline);
        
    void cmdBindVertexBuffers(
        VkCommandBuffer commandBuffer,
        uint32_t firstBinding,
        uint32_t bindingCount,
        const VkBuffer* pBuffers,
        const VkDeviceSize* pOffsets);
        
    void cmdBindIndexBuffer(
        VkCommandBuffer commandBuffer,
        VkBuffer buffer,
        VkDeviceSize offset,
        VkIndexType indexType);
        
    void cmdBindDescriptorSets(
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipelineLayout layout,
        uint32_t firstSet,
        uint32_t descriptorSetCount,
        const VkDescriptorSet* pDescriptorSets,
        uint32_t dynamicOffsetCount,
        const uint32_t* pDynamicOffsets);
        
    void cmdDraw(
        VkCommandBuffer commandBuffer,
        uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t firstVertex,
        uint32_t firstInstance);
        
    void cmdDrawIndexed(
        VkCommandBuffer commandBuffer,
        uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t firstIndex,
        int32_t vertexOffset,
        uint32_t firstInstance);
        
    // Pipeline
    VkResult createGraphicsPipelines(
        VkPipelineCache pipelineCache,
        uint32_t createInfoCount,
        const VkGraphicsPipelineCreateInfo* pCreateInfos,
        const VkAllocationCallbacks* pAllocator,
        VkPipeline* pPipelines);
        
    void destroyPipeline(
        VkPipeline pipeline,
        const VkAllocationCallbacks* pAllocator);
        
    // Pipeline Layout
    VkResult createPipelineLayout(
        const VkPipelineLayoutCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkPipelineLayout* pPipelineLayout);
        
    void destroyPipelineLayout(
        VkPipelineLayout pipelineLayout,
        const VkAllocationCallbacks* pAllocator);
        
    // Descriptor Set Layout
    VkResult createDescriptorSetLayout(
        const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDescriptorSetLayout* pSetLayout);
        
    void destroyDescriptorSetLayout(
        VkDescriptorSetLayout descriptorSetLayout,
        const VkAllocationCallbacks* pAllocator);
        
    // Descriptor Pool
    VkResult createDescriptorPool(
        const VkDescriptorPoolCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDescriptorPool* pDescriptorPool);
        
    void destroyDescriptorPool(
        VkDescriptorPool descriptorPool,
        const VkAllocationCallbacks* pAllocator);
        
    // Descriptor Sets
    VkResult allocateDescriptorSets(
        const VkDescriptorSetAllocateInfo* pAllocateInfo,
        VkDescriptorSet* pDescriptorSets);
        
    void updateDescriptorSets(
        uint32_t descriptorWriteCount,
        const VkWriteDescriptorSet* pDescriptorWrites,
        uint32_t descriptorCopyCount,
        const VkCopyDescriptorSet* pDescriptorCopies);
        
    // Shader Module
    VkResult createShaderModule(
        const VkShaderModuleCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkShaderModule* pShaderModule);
        
    void destroyShaderModule(
        VkShaderModule shaderModule,
        const VkAllocationCallbacks* pAllocator);
        
    // Semaphore and Fence
    VkResult createSemaphore(
        const VkSemaphoreCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkSemaphore* pSemaphore);
        
    void destroySemaphore(
        VkSemaphore semaphore,
        const VkAllocationCallbacks* pAllocator);
        
    VkResult createFence(
        const VkFenceCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkFence* pFence);
        
    void destroyFence(
        VkFence fence,
        const VkAllocationCallbacks* pAllocator);
        
    VkResult waitForFences(
        uint32_t fenceCount,
        const VkFence* pFences,
        VkBool32 waitAll,
        uint64_t timeout);
        
    VkResult resetFences(
        uint32_t fenceCount,
        const VkFence* pFences);

    // Swapchain operations
    VkResult acquireNextImageKHR(
        VkSwapchainKHR swapchain,
        uint64_t timeout,
        VkSemaphore semaphore,
        VkFence fence,
        uint32_t* pImageIndex);

    // Command buffer operations
    VkResult resetCommandBuffer(
        VkCommandBuffer commandBuffer,
        VkCommandBufferResetFlags flags);

    // Viewport and scissor operations
    void cmdSetViewport(
        VkCommandBuffer commandBuffer,
        uint32_t firstViewport,
        uint32_t viewportCount,
        const VkViewport* pViewports);

    void cmdSetScissor(
        VkCommandBuffer commandBuffer,
        uint32_t firstScissor,
        uint32_t scissorCount,
        const VkRect2D* pScissors);

    // Command operations
    void cmdCopyBuffer(
        VkCommandBuffer commandBuffer,
        VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        uint32_t regionCount,
        const VkBufferCopy* pRegions);
        
    void cmdPipelineBarrier(
        VkCommandBuffer commandBuffer,
        VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask,
        VkDependencyFlags dependencyFlags,
        uint32_t memoryBarrierCount,
        const VkMemoryBarrier* pMemoryBarriers,
        uint32_t bufferMemoryBarrierCount,
        const VkBufferMemoryBarrier* pBufferMemoryBarriers,
        uint32_t imageMemoryBarrierCount,
        const VkImageMemoryBarrier* pImageMemoryBarriers);

    // Queue operations
    VkResult queueSubmit(
        uint32_t submitCount,
        const VkSubmitInfo* pSubmits,
        VkFence fence);

    VkResult queuePresentKHR(
        const VkPresentInfoKHR* pPresentInfo);

    VkResult queueWaitIdle();

    void recreateSwapChain();

    // Descriptor operations
    VkResult resetDescriptorPool(
        VkDescriptorPool descriptorPool,
        VkDescriptorPoolResetFlags flags);

    // Format properties
    void getPhysicalDeviceFormatProperties(
        VkFormat format,
        VkFormatProperties* pFormatProperties);

    void cmdCopyBufferToImage(
        VkCommandBuffer commandBuffer,
        VkBuffer buffer,
        VkImage image,
        VkImageLayout imageLayout,
        uint32_t regionCount,
        const VkBufferImageCopy* pRegions);

    void getPhysicalDeviceProperties(
        VkPhysicalDeviceProperties* pProperties);

    VkResult createSampler(
        const VkSamplerCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkSampler* pSampler);

    void destroySampler(
        VkSampler sampler,
        const VkAllocationCallbacks* pAllocator);

private:

    void initVulkan();
    void shutDownVulkan();

    void pickPhysicalDevice();
    int rateDeviceSuitability(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    void createLogicalDevice();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);


    void createSwapChain();
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    
    void cleanupSwapChain();

    void createImageViews();

    void createRenderPass();

    void createFrameBuffers();

    void createDepthBuffer();

    void createCommandPool();
private:
    const int MAX_FRAMES_IN_FLIGHT = 2;

    Window& m_window;

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device;

    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    VkSwapchainKHR m_swapChain;
    std::vector<VkImage> m_swapChainImages;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    std::vector<VkImageView> m_swapChainImageViews;

    VkRenderPass m_renderPass;

    std::vector<VkFramebuffer> m_swapChainFramebuffers;

    ::Engine::Ressources::Texture* m_depthBuffer;

    bool m_framebufferResized = false;

    VkCommandPool m_commandPool;

    std::vector<std::function<void(VkRenderPass&, VkExtent2D&)>> m_recreationCallbacks;
};


}
}
