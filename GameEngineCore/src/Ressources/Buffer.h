#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

namespace Engine {
namespace Ressources {

class Buffer {
public:
    Buffer(uint32_t bufferCount = 1);
    
    virtual ~Buffer();

    virtual void updateData(void* data, size_t size, uint32_t bufferIndex = 0, uint32_t offset = 0) = 0;
    VkBuffer getBuffer(uint32_t index = 0) const { return m_buffers[index]; }

protected:
    void createBaseBuffer(
        size_t size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        uint32_t index
    );
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0);

protected:
    std::vector<VkBuffer> m_buffers;
    std::vector<VkDeviceMemory> m_buffersMemory;
    std::vector<void*> m_mappedMemory;  // For persistent mapping if needed

private:
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

}
}
