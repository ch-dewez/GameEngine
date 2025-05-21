#include "Core/Renderer/VulkanApi.h"
#include "UniformBuffer.h"
#include <stdexcept>
#include <cstring>

namespace Engine {
namespace Ressources {

UniformBuffer::UniformBuffer(size_t size, uint32_t bufferCount)
    : Buffer(size, bufferCount)
{
    createBuffer(size);
}

void UniformBuffer::createBuffer(size_t size) {
    Renderer::VulkanApi& api = Renderer::VulkanApi::Instance();
    for (size_t i = 0; i < m_buffers.size(); i++) {
        createBaseBuffer(
            size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            i
        );

        VkResult result = api.mapMemory(m_buffersMemory[i], 0, size, 0, &m_mappedMemory[i]);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to map memory for uniform buffer");
        }
    }
}

void UniformBuffer::updateData(void* data, size_t size, uint32_t bufferIndex, uint32_t offset) {
    if (offset + size > m_size) {
        throw std::runtime_error("Update data size + offset larger than buffer size");
    }
    
    if (bufferIndex >= m_mappedMemory.size()) {
        throw std::runtime_error("Buffer index out of range");
    }
    
    // Copy data to the correct offset in the mapped memory
    char* dst = static_cast<char*>(m_mappedMemory[bufferIndex]) + offset;
    memcpy(dst, data, size);
}

}
}
