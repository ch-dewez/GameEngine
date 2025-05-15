#include "IndexBuffer.h"
#include "Core/Renderer/VulkanApi.h"
#include "vulkan/vulkan_core.h"
#include <cstddef>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace Engine {
namespace Ressources {

IndexBuffer::IndexBuffer(void* data, size_t size, int indexCount)
    : Buffer(1), indexCount(indexCount)
{
    createBuffer(data, size);
}

void IndexBuffer::createBuffer(void* data, size_t size) {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBaseBuffer(
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        0  // Use temporary index
    );

    stagingBuffer = m_buffers[0];
    stagingBufferMemory = m_buffersMemory[0];

    void* mappedMemory;
    api.mapMemory(stagingBufferMemory, 0, size, 0, &mappedMemory);
    memcpy(mappedMemory, data, size);
    api.unmapMemory(stagingBufferMemory);

    createBaseBuffer(
        size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        0  // Use index 0 for the main buffer
    );

    copyBuffer(stagingBuffer, m_buffers[0], size);

    api.destroyBuffer(stagingBuffer, nullptr);
    api.freeMemory(stagingBufferMemory, nullptr);
}

void IndexBuffer::updateData(void* data, size_t size, uint32_t bufferIndex, uint32_t offset) {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBaseBuffer(
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        0  // Use temporary index
    );

    stagingBuffer = m_buffers[0];
    stagingBufferMemory = m_buffersMemory[0];

    void* mappedMemory;
    api.mapMemory(stagingBufferMemory, 0, size, 0, &mappedMemory);
    memcpy(mappedMemory, data, size);
    api.unmapMemory(stagingBufferMemory);

    copyBuffer(stagingBuffer, m_buffers[bufferIndex], size, 0, offset);

    api.destroyBuffer(stagingBuffer, nullptr);
    api.freeMemory(stagingBufferMemory, nullptr);
}

}
}
