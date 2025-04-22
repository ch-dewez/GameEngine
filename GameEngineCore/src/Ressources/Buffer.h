#pragma once
#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>
#include <vector>
#include "Renderer/VulkanApi.h"

namespace Engine {
namespace Ressources {

class Buffer {
public:
    Buffer(uint32_t bufferCount = 1);
    
    ~Buffer();

    virtual void updateData(void* data, size_t size, uint32_t bufferIndex = 0, uint32_t offset = 0);
    VkBuffer getBuffer(uint32_t index = 0) const { return m_buffers[index]; }

    void createBaseBuffer(
        size_t size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        uint32_t index
    );
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0);

    void mapMemory(VkDeviceSize size, VkMemoryMapFlags flags, void** ppData, uint32_t index = 0);
    void unmapMemory(uint32_t index = 0);

protected:
    std::vector<VkBuffer> m_buffers;
    std::vector<VkDeviceMemory> m_buffersMemory;
    std::vector<void*> m_mappedMemory;  // For persistent mapping if needed

public:
    static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, Renderer::VulkanApi& api);
};

}
}
