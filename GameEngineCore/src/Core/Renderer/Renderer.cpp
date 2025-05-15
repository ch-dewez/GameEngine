#include "Renderer.h"
#include "Core/Ressources/DescriptorsManager.h"
#include "VulkanApi.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <iostream>


namespace Engine {
namespace Renderer {

Renderer::Renderer() {
    createCommandBuffers();
    createSyncObjects();
}

Renderer::~Renderer() {
    VulkanApi& api = VulkanApi::Instance();
    for (size_t i = 0; i < api.getMaxFramesInFlight(); i++) {
        api.destroySemaphore(m_renderFinishedSemaphores[i], nullptr);
        api.destroySemaphore(m_imageAvailableSemaphores[i], nullptr);
        api.destroyFence(m_inFlightFences[i], nullptr);
    }
}


void Renderer::beginFrame() {
    VulkanApi& api = VulkanApi::Instance();
    api.waitForFences(1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);


    VkResult result = api.acquireNextImageKHR(api.getSwapChain(), UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        api.recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS &&
        result != VK_SUBOPTIMAL_KHR) { // VK suboptimal consider as success
        // but can be consider as failure
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    api.resetFences(1,  &m_inFlightFences[m_currentFrame]);

    vkResetCommandBuffer(m_commandBuffers[m_currentFrame], 0);

    m_frameInfo.commandBuffer = m_commandBuffers[m_currentFrame];
    m_frameInfo.frameIndex = m_currentFrame;
}

void Renderer::beginRenderPass() {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    VulkanApi& api = VulkanApi::Instance();
    
    if (api.beginCommandBuffer(m_commandBuffers[m_currentFrame], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = api.getRenderPass();
    renderPassInfo.framebuffer = api.getSwapChainFrameBuffer(m_currentImageIndex);

    renderPassInfo.renderArea.offset = {0, 0};
    auto swapChainExtent = api.getSwapChainExtent();
    renderPassInfo.renderArea.extent = swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    api.cmdBeginRenderPass(m_commandBuffers[m_currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // we set them as dynamic so we have to set them
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    api.cmdSetViewport(m_commandBuffers[m_currentFrame], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;
    api.cmdSetScissor(m_commandBuffers[m_currentFrame], 0, 1, &scissor);

}

void Renderer::endRenderPass() {
    VulkanApi& api = VulkanApi::Instance();
    api.cmdEndRenderPass(m_commandBuffers[m_currentFrame]);

    if (api.endCommandBuffer(m_commandBuffers[m_currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void Renderer::endFrame() {
    VulkanApi& api = VulkanApi::Instance();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];

    VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (api.queueSubmit(1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {api.getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_currentImageIndex;
    presentInfo.pResults = nullptr; // Optional

    VkResult result = api.queuePresentKHR(&presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        api.frameBufferResized()) {
        api.setFrameBufferResized(false);
        api.recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % api.getMaxFramesInFlight();
}

void Renderer::createCommandBuffers() {
    VulkanApi& api = VulkanApi::Instance();
    auto maxFrameInFlight = api.getMaxFramesInFlight();

    m_commandBuffers.resize(maxFrameInFlight);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = api.getCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = maxFrameInFlight;

    if (api.allocateCommandBuffers(&allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
};


void Renderer::createSyncObjects() {
    VulkanApi& api = VulkanApi::Instance();
    auto maxFrameInFlight = api.getMaxFramesInFlight();

    m_imageAvailableSemaphores.resize(maxFrameInFlight, VK_NULL_HANDLE);
    m_renderFinishedSemaphores.resize(maxFrameInFlight, VK_NULL_HANDLE);
    m_inFlightFences.resize(maxFrameInFlight, VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    // this set the signaled bit when we create because at the first frame signaled bit would never be triggered
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; 

    for (size_t i = 0; i < maxFrameInFlight; i++) {
        if (api.createSemaphore(&semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS || 
            api.createSemaphore(&semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            api.createFence(&fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
};


}
}


