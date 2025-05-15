#pragma once
#include <GLFW/glfw3.h>
#include "Buffer.h"


namespace Engine {
namespace Ressources {

class VertexBuffer: public Buffer {
public:
    VertexBuffer(void* data, size_t size);
                 
    void createBuffer(void* data, size_t size);
    void updateData(void* data, size_t size, uint32_t bufferIndex = 0, uint32_t offset = 0) override;
};

}
}
