#include "VulkanApi.h"
#include "VulkanConfig.h"
#include "vulkan/vulkan_core.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <set>

namespace Engine {
namespace Renderer {


VulkanApi* vulkanInstance = nullptr;
VulkanApi& VulkanApi::Instance() {
    if (!vulkanInstance) {
        throw std::runtime_error("VulkanApi not initialized - call Init() first");
    }
    return *vulkanInstance;
}
void VulkanApi::Init(Window& window) {
    vulkanInstance = new VulkanApi(window);
}
void VulkanApi::Shutdown(){
    delete vulkanInstance;
    vulkanInstance = nullptr;
}

VulkanApi::VulkanApi(Window& window)
: m_window(window)
{
    window.setResizeCallback([this](int width, int height) {
        m_framebufferResized = true;
    });
    initVulkan();
}

VulkanApi::~VulkanApi() {
    shutDownVulkan();
}


void VulkanApi::initVulkan() {
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createFrameBuffers();
    createCommandPool();
}

void VulkanApi::shutDownVulkan() {

    cleanupSwapChain();

    //delete m_depthBuffer;

    vkDestroyRenderPass(m_device, m_renderPass, nullptr);

    
    vkDestroyCommandPool(m_device, m_commandPool, nullptr);

    vkDestroyDevice(m_device, nullptr);
}

void VulkanApi::deviceWaitIdle() {
    vkDeviceWaitIdle(m_device);
}


void VulkanApi::pickPhysicalDevice(){
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_window.getInstance(), &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_window.getInstance(), &deviceCount, devices.data());

    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device : devices) {
        int score = rateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0) {
        m_physicalDevice = candidates.rbegin()->second;
    } else {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

int VulkanApi::rateDeviceSuitability(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    if (! findQueueFamilies(device).isComplete()) return 0; 

    if (! checkDeviceExtensionSupport(device)) return 0;

    bool swapChainAdequate = false;
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
    swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

    if (! swapChainAdequate) return 0; 

    // if the device has met the previous condition it is suitable
    int score = 1;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // idk if it's good but it is in the vulkan documentations
    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}

bool VulkanApi::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(VulkanConfig::DeviceExtensions.begin(), VulkanConfig::DeviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
};

void VulkanApi::createLogicalDevice() {
    
    QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    // this is now optional with newer version of vulkan
    createInfo.enabledExtensionCount = 0;
    if (VulkanConfig::EnableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanConfig::ValidationLayers.size());
        createInfo.ppEnabledLayerNames = VulkanConfig::ValidationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(VulkanConfig::DeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = VulkanConfig::DeviceExtensions.data();


    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    // get queue handles
    vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
}


QueueFamilyIndices VulkanApi::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_window.getSurface(), &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

void VulkanApi::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    // Use minimum image count + 1 for triple buffering, but don't exceed max
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && 
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // Prefer FIFO present mode on macOS
    presentMode = VK_PRESENT_MODE_FIFO_KHR;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_window.getSurface();
    
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    // Handle queue families
    QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());

    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
}


VkSurfaceFormatKHR VulkanApi::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    // we could rank all the format but it's fine to just take the first one
    return availableFormats[0];
};

VkPresentModeKHR VulkanApi::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanApi::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(m_window.getGLFWWindow(), &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

SwapChainSupportDetails VulkanApi::querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_window.getSurface(), &details.capabilities);


    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_window.getSurface(), &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_window.getSurface(), &formatCount, details.formats.data());
    }


    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_window.getSurface(), &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_window.getSurface(), &presentModeCount, details.presentModes.data());
    }

    return details;
}

void VulkanApi::cleanupSwapChain(){
    for (size_t i = 0; i < m_swapChainFramebuffers.size(); i++) {
        vkDestroyFramebuffer(m_device, m_swapChainFramebuffers[i], nullptr);
    }

    for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
        vkDestroyImageView(m_device, m_swapChainImageViews[i], nullptr);
    }

    //delete m_depthBuffer;
    //m_depthBuffer = nullptr;

    vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
};

void VulkanApi::recreateSwapChain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_window.getGLFWWindow(), &width, &height);
    // if window minimized (== 0) then wait until reopened
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_window.getGLFWWindow(), &width, &height);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(m_device);

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    //createDepthResources();
    createFrameBuffers();

    // Call all recreation callbacks
    for (auto& callback : m_recreationCallbacks) {
        callback(m_renderPass, m_swapChainExtent);
    }
}


void VulkanApi::createImageViews() {
    m_swapChainImageViews.resize(m_swapChainImages.size());

    for (size_t i = 0; i < m_swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;  // Changed from 1D to 2D
        createInfo.format = m_swapChainImageFormat;   // Use the swap chain format

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_device, &createInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

/*void VulkanApi::createDepthResources() {*/
/*    m_depthFormat = Ressources::ImageBuffer::findDepthFormat();*/
/**/
/*    m_depthBuffer = new Ressources::ImageBuffer(*/
/*        &m_device,*/
/*        &m_physicalDevice,*/
/*        m_swapChainExtent.width,*/
/*        m_swapChainExtent.height,*/
/*        m_depthFormat,*/
/*        VK_IMAGE_TILING_OPTIMAL,*/
/*        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,*/
/*        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT*/
/*    );*/
/**/
/*    m_depthBuffer->createImageView(*/
/*        m_depthFormat,*/
/*        VK_IMAGE_ASPECT_DEPTH_BIT*/
/*    );*/
/*}*/

void VulkanApi::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    /*VkAttachmentDescription depthAttachment{};*/
    /*depthAttachment.format = m_depthFormat;*/
    /*depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;*/
    /*depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;*/
    /*depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;*/
    /*depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;*/
    /*depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;*/
    /*depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;*/
    /*depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;*/

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    /*VkAttachmentReference depthAttachmentRef{};*/
    /*depthAttachmentRef.attachment = 1;*/
    /*depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;*/

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    //subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    //std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    std::array<VkAttachmentDescription, 1> attachments = {colorAttachment};
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void VulkanApi::createFrameBuffers() {
    m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

    for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
        std::array<VkImageView, 1> attachments = {
            m_swapChainImageViews[i],
            //m_depthBuffer->getImageView()
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_swapChainExtent.width;
        framebufferInfo.height = m_swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void VulkanApi::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
};

VkResult VulkanApi::createBuffer(
    const VkBufferCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkBuffer* pBuffer) 
{
    return vkCreateBuffer(m_device, pCreateInfo, pAllocator, pBuffer);
}

void VulkanApi::destroyBuffer(
    VkBuffer buffer,
    const VkAllocationCallbacks* pAllocator) 
{
    vkDestroyBuffer(m_device, buffer, pAllocator);
}

VkResult VulkanApi::allocateMemory(
    const VkMemoryAllocateInfo* pAllocateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDeviceMemory* pMemory) 
{
    return vkAllocateMemory(m_device, pAllocateInfo, pAllocator, pMemory);
}

void VulkanApi::freeMemory(
    VkDeviceMemory memory,
    const VkAllocationCallbacks* pAllocator) 
{
    vkFreeMemory(m_device, memory, pAllocator);
}

VkResult VulkanApi::allocateCommandBuffers(
    const VkCommandBufferAllocateInfo* pAllocateInfo,
    VkCommandBuffer* pCommandBuffers) 
{
    return vkAllocateCommandBuffers(m_device, pAllocateInfo, pCommandBuffers);
}

void VulkanApi::freeCommandBuffers(
    uint32_t commandBufferCount,
    const VkCommandBuffer* pCommandBuffers) 
{
    vkFreeCommandBuffers(m_device, m_commandPool, commandBufferCount, pCommandBuffers);
}

VkResult VulkanApi::createImage(
    const VkImageCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkImage* pImage) 
{
    return vkCreateImage(m_device, pCreateInfo, pAllocator, pImage);
}

void VulkanApi::destroyImage(
    VkImage image,
    const VkAllocationCallbacks* pAllocator) 
{
    vkDestroyImage(m_device, image, pAllocator);
}

VkResult VulkanApi::createImageView(
    const VkImageViewCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkImageView* pView) 
{
    return vkCreateImageView(m_device, pCreateInfo, pAllocator, pView);
}

void VulkanApi::destroyImageView(
    VkImageView imageView,
    const VkAllocationCallbacks* pAllocator) 
{
    vkDestroyImageView(m_device, imageView, pAllocator);
}

VkResult VulkanApi::beginCommandBuffer(
    VkCommandBuffer commandBuffer,
    const VkCommandBufferBeginInfo* pBeginInfo)
{
    return vkBeginCommandBuffer(commandBuffer, pBeginInfo);
}

VkResult VulkanApi::endCommandBuffer(VkCommandBuffer commandBuffer)
{
    return vkEndCommandBuffer(commandBuffer);
}

void VulkanApi::cmdBeginRenderPass(
    VkCommandBuffer commandBuffer,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    VkSubpassContents contents)
{
    vkCmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
}

void VulkanApi::cmdEndRenderPass(VkCommandBuffer commandBuffer)
{
    vkCmdEndRenderPass(commandBuffer);
}

void VulkanApi::cmdBindPipeline(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipeline pipeline)
{
    vkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
}

void VulkanApi::cmdBindVertexBuffers(
    VkCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const VkBuffer* pBuffers,
    const VkDeviceSize* pOffsets)
{
    vkCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
}

void VulkanApi::cmdBindIndexBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkIndexType indexType)
{
    vkCmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
}

void VulkanApi::cmdBindDescriptorSets(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipelineLayout layout,
    uint32_t firstSet,
    uint32_t descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets)
{
    vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet,
        descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
}

void VulkanApi::cmdDraw(
    VkCommandBuffer commandBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance)
{
    vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanApi::cmdDrawIndexed(
    VkCommandBuffer commandBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance)
{
    vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex,
        vertexOffset, firstInstance);
}

VkResult VulkanApi::createGraphicsPipelines(
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkGraphicsPipelineCreateInfo* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines)
{
    return vkCreateGraphicsPipelines(m_device, pipelineCache, createInfoCount,
        pCreateInfos, pAllocator, pPipelines);
}

void VulkanApi::destroyPipeline(
    VkPipeline pipeline,
    const VkAllocationCallbacks* pAllocator)
{
    vkDestroyPipeline(m_device, pipeline, pAllocator);
}

VkResult VulkanApi::createPipelineLayout(
    const VkPipelineLayoutCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkPipelineLayout* pPipelineLayout)
{
    return vkCreatePipelineLayout(m_device, pCreateInfo, pAllocator, pPipelineLayout);
}

void VulkanApi::destroyPipelineLayout(
    VkPipelineLayout pipelineLayout,
    const VkAllocationCallbacks* pAllocator)
{
    vkDestroyPipelineLayout(m_device, pipelineLayout, pAllocator);
}

VkResult VulkanApi::createDescriptorSetLayout(
    const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorSetLayout* pSetLayout)
{
    return vkCreateDescriptorSetLayout(m_device, pCreateInfo, pAllocator, pSetLayout);
}

void VulkanApi::destroyDescriptorSetLayout(
    VkDescriptorSetLayout descriptorSetLayout,
    const VkAllocationCallbacks* pAllocator)
{
    vkDestroyDescriptorSetLayout(m_device, descriptorSetLayout, pAllocator);
}

VkResult VulkanApi::createDescriptorPool(
    const VkDescriptorPoolCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorPool* pDescriptorPool)
{
    return vkCreateDescriptorPool(m_device, pCreateInfo, pAllocator, pDescriptorPool);
}

void VulkanApi::destroyDescriptorPool(
    VkDescriptorPool descriptorPool,
    const VkAllocationCallbacks* pAllocator)
{
    vkDestroyDescriptorPool(m_device, descriptorPool, pAllocator);
}

VkResult VulkanApi::allocateDescriptorSets(
    const VkDescriptorSetAllocateInfo* pAllocateInfo,
    VkDescriptorSet* pDescriptorSets)
{
    return vkAllocateDescriptorSets(m_device, pAllocateInfo, pDescriptorSets);
}

void VulkanApi::updateDescriptorSets(
    uint32_t descriptorWriteCount,
    const VkWriteDescriptorSet* pDescriptorWrites,
    uint32_t descriptorCopyCount,
    const VkCopyDescriptorSet* pDescriptorCopies)
{
    vkUpdateDescriptorSets(m_device, descriptorWriteCount, pDescriptorWrites,
        descriptorCopyCount, pDescriptorCopies);
}

VkResult VulkanApi::createShaderModule(
    const VkShaderModuleCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkShaderModule* pShaderModule)
{
    return vkCreateShaderModule(m_device, pCreateInfo, pAllocator, pShaderModule);
}

void VulkanApi::destroyShaderModule(
    VkShaderModule shaderModule,
    const VkAllocationCallbacks* pAllocator)
{
    vkDestroyShaderModule(m_device, shaderModule, pAllocator);
}

VkResult VulkanApi::createSemaphore(
    const VkSemaphoreCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSemaphore* pSemaphore)
{

    return vkCreateSemaphore(m_device, pCreateInfo, pAllocator, pSemaphore);
}

void VulkanApi::destroySemaphore(
    VkSemaphore semaphore,
    const VkAllocationCallbacks* pAllocator)
{
    vkDestroySemaphore(m_device, semaphore, pAllocator);
}

VkResult VulkanApi::createFence(
    const VkFenceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFence* pFence)
{
    return vkCreateFence(m_device, pCreateInfo, pAllocator, pFence);
}

void VulkanApi::destroyFence(
    VkFence fence,
    const VkAllocationCallbacks* pAllocator)
{
    vkDestroyFence(m_device, fence, pAllocator);
}

VkResult VulkanApi::waitForFences(
    uint32_t fenceCount,
    const VkFence* pFences,
    VkBool32 waitAll,
    uint64_t timeout)
{
    return vkWaitForFences(m_device, fenceCount, pFences, waitAll, timeout);
}

VkResult VulkanApi::resetFences(
    uint32_t fenceCount,
    const VkFence* pFences)
{
    return vkResetFences(m_device, fenceCount, pFences);
}

VkResult VulkanApi::acquireNextImageKHR(
    VkSwapchainKHR swapchain,
    uint64_t timeout,
    VkSemaphore semaphore,
    VkFence fence,
    uint32_t* pImageIndex)
{
    return vkAcquireNextImageKHR(m_device, swapchain, timeout, semaphore, fence, pImageIndex);
}

VkResult VulkanApi::resetCommandBuffer(
    VkCommandBuffer commandBuffer,
    VkCommandBufferResetFlags flags)
{
    return vkResetCommandBuffer(commandBuffer, flags);
}

void VulkanApi::cmdSetViewport(
    VkCommandBuffer commandBuffer,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const VkViewport* pViewports)
{
    vkCmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
}

void VulkanApi::cmdSetScissor(
    VkCommandBuffer commandBuffer,
    uint32_t firstScissor,
    uint32_t scissorCount,
    const VkRect2D* pScissors)
{
    vkCmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
}

VkResult VulkanApi::queueSubmit(
    uint32_t submitCount,
    const VkSubmitInfo* pSubmits,
    VkFence fence)
{
    return vkQueueSubmit(m_graphicsQueue, submitCount, pSubmits, fence);
}

VkResult VulkanApi::queuePresentKHR(
    const VkPresentInfoKHR* pPresentInfo)
{
    return vkQueuePresentKHR(m_presentQueue, pPresentInfo);
}

void VulkanApi::unmapMemory(VkDeviceMemory memory) {
    vkUnmapMemory(m_device, memory);
}

void VulkanApi::getBufferMemoryRequirements(
    VkBuffer buffer,
    VkMemoryRequirements* pMemoryRequirements) 
{
    vkGetBufferMemoryRequirements(m_device, buffer, pMemoryRequirements);
}

VkResult VulkanApi::bindBufferMemory(
    VkBuffer buffer,
    VkDeviceMemory memory,
    VkDeviceSize memoryOffset) 
{
    return vkBindBufferMemory(m_device, buffer, memory, memoryOffset);
}

void VulkanApi::cmdCopyBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    uint32_t regionCount,
    const VkBufferCopy* pRegions) 
{
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
}

VkResult VulkanApi::queueWaitIdle() {
    return vkQueueWaitIdle(m_graphicsQueue);
}

void VulkanApi::getPhysicalDeviceMemoryProperties(
    VkPhysicalDeviceMemoryProperties* pMemoryProperties) 
{
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, pMemoryProperties);
}

VkResult VulkanApi::resetDescriptorPool(
    VkDescriptorPool descriptorPool,
    VkDescriptorPoolResetFlags flags) 
{
    return vkResetDescriptorPool(m_device, descriptorPool, flags);
}

void VulkanApi::getImageMemoryRequirements(
    VkImage image,
    VkMemoryRequirements* pMemoryRequirements) 
{
    vkGetImageMemoryRequirements(m_device, image, pMemoryRequirements);
}

VkResult VulkanApi::bindImageMemory(
    VkImage image,
    VkDeviceMemory memory,
    VkDeviceSize memoryOffset) 
{
    return vkBindImageMemory(m_device, image, memory, memoryOffset);
}

void VulkanApi::cmdPipelineBarrier(
    VkCommandBuffer commandBuffer,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask,
    VkDependencyFlags dependencyFlags,
    uint32_t memoryBarrierCount,
    const VkMemoryBarrier* pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const VkImageMemoryBarrier* pImageMemoryBarriers) 
{
    vkCmdPipelineBarrier(
        commandBuffer,
        srcStageMask,
        dstStageMask,
        dependencyFlags,
        memoryBarrierCount,
        pMemoryBarriers,
        bufferMemoryBarrierCount,
        pBufferMemoryBarriers,
        imageMemoryBarrierCount,
        pImageMemoryBarriers
    );
}

VkResult VulkanApi::mapMemory(
    VkDeviceMemory memory,
    VkDeviceSize offset,
    VkDeviceSize size,
    VkMemoryMapFlags flags,
    void** ppData) 
{
    return vkMapMemory(m_device, memory, offset, size, flags, ppData);
}

void VulkanApi::getPhysicalDeviceFormatProperties(
    VkFormat format,
    VkFormatProperties* pFormatProperties) 
{
    vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, pFormatProperties);
}

}
}
