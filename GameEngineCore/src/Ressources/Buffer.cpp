#include "Buffer.h"
#include "Renderer/VulkanApi.h"
#include "vulkan/vulkan_core.h"
#include <stdexcept>

namespace Engine {
namespace Ressources {

Buffer::Buffer(uint32_t bufferCount)
: m_buffers(bufferCount, VK_NULL_HANDLE)  // Initialize with null handles
    , m_buffersMemory(bufferCount, VK_NULL_HANDLE)  // Initialize with null handles
    , m_mappedMemory(bufferCount, nullptr) 
{
}

Buffer::~Buffer() {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    for (size_t i = 0; i < m_buffers.size(); i++) {
        if (m_mappedMemory[i]) {
            api.unmapMemory(m_buffersMemory[i]);
        }
        api.destroyBuffer(m_buffers[i], nullptr);
        api.freeMemory(m_buffersMemory[i], nullptr);
    }
}

void Buffer::createBaseBuffer(
    size_t size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    uint32_t bufferIndex
) {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (api.createBuffer(&bufferInfo, nullptr, &m_buffers[bufferIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }; 

    VkMemoryRequirements memRequirements;
    api.getBufferMemoryRequirements(m_buffers[bufferIndex], &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (api.allocateMemory(&allocInfo, nullptr, &m_buffersMemory[bufferIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    };

    api.bindBufferMemory(m_buffers[bufferIndex], m_buffersMemory[bufferIndex], 0);
}

void Buffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset) {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = api.getCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    api.allocateCommandBuffers(&allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    api.beginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = srcOffset;
    copyRegion.dstOffset = dstOffset;
    copyRegion.size = size;
    api.cmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    api.endCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    api.queueSubmit(1, &submitInfo, VK_NULL_HANDLE);
    api.queueWaitIdle();

    api.freeCommandBuffers(1, &commandBuffer);
}

uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    VkPhysicalDeviceMemoryProperties memProperties;
    api.getPhysicalDeviceMemoryProperties(&memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && 
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

}
}
