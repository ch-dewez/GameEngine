#pragma once
#include <GLFW/glfw3.h>
#include "Buffer.h"

namespace Engine {
namespace Ressources {

class UniformBuffer: public Buffer {
public:
    UniformBuffer(size_t size, uint32_t bufferCount = 1);
                 
    void createBuffer(size_t size);
    void updateData(void* data, size_t size, uint32_t bufferIndex = 0, uint32_t offset=0) override;

private:
    size_t m_bufferSize;
};

}
}
