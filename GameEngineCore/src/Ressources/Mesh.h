#pragma once
#include "Renderer/Renderer.h"
#include "IndexBuffer.h"
#include "vertexBuffer.h"
#include <memory>
#include <string>
#include <vector>

namespace Engine {
namespace Ressources {

class Mesh {
public:
    // TODO: load obj
    Mesh(std::string& path) = delete;

    Mesh(std::vector<uint32_t> indices, void* vertexData, size_t size);

    void bind(Engine::Renderer::Renderer::FrameInfo frameInfo);
    void draw(Engine::Renderer::Renderer::FrameInfo frameInfo);
private:
    std::unique_ptr<IndexBuffer> m_indexBuffer;
    std::unique_ptr<VertexBuffer> m_vertexBuffer;
};

}
}
