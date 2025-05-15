#pragma once
#include <GLFW/glfw3.h>
#include "Buffer.h"


namespace Engine {
namespace Ressources {

class IndexBuffer: public Buffer {
public:
    IndexBuffer(void* data, size_t size, int indexCount);
                
    void createBuffer(void* data, size_t size);
    void updateData(void* data, size_t size, uint32_t bufferIndex = 0, uint32_t offset = 0) override;

    int indexCount;
};

}
}
