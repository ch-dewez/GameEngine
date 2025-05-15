#include "Buffer.h"
#include "Core/Renderer/VulkanApi.h"
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
    VkCommandBuffer commandBuffer = api.beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = srcOffset;
    copyRegion.dstOffset = dstOffset;
    copyRegion.size = size;
    api.cmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    api.endSingleTimeCommands(commandBuffer);
}


uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)  {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    return findMemoryType(typeFilter, properties, api);
}

uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, Renderer::VulkanApi& api) {
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

void Buffer::mapMemory(VkDeviceSize size, VkMemoryMapFlags flags, void** ppData, uint32_t index) {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    api.mapMemory(m_buffersMemory[index], 0, size, flags, ppData);
}
void Buffer::unmapMemory(uint32_t index) {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    api.unmapMemory(m_buffersMemory[index]);
}

void Buffer::updateData(void* data, size_t size, uint32_t bufferIndex, uint32_t offset) {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    // Create staging buffer
    createBaseBuffer(
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        0  // Use temporary index
    );

    stagingBuffer = m_buffers[0];
    stagingBufferMemory = m_buffersMemory[0];

    // Map memory and copy data
    void* mappedMemory;
    api.mapMemory(stagingBufferMemory, 0, size, 0, &mappedMemory);
    memcpy(mappedMemory, data, size);
    api.unmapMemory(stagingBufferMemory);

    // Copy from staging buffer to device local buffer
    copyBuffer(stagingBuffer, m_buffers[bufferIndex], size, 0, offset);

    // Cleanup staging buffer
    api.destroyBuffer(stagingBuffer, nullptr);
    api.freeMemory(stagingBufferMemory, nullptr);
}

}
}
