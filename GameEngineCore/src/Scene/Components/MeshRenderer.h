#include "Renderer.h"
#include <memory>
#include "Ressources/vertexBuffer.h"
#include "Ressources/IndexBuffer.h"
#include "Renderer/Renderer.h"

namespace Engine {
namespace Components {

class MeshRenderer: public Renderer {
public:

    MeshRenderer(std::shared_ptr<Ressources::Material> material, std::unique_ptr<Ressources::VertexBuffer> vertexBufferKey, std::unique_ptr<Ressources::IndexBuffer> indexBufferKey);

    void render(Engine::Renderer::Renderer::FrameInfo& frameInfo) override;

private:
    std::unique_ptr<Ressources::VertexBuffer> m_vertexBuffer;
    std::unique_ptr<Ressources::IndexBuffer> m_indexBuffer;
};


}
}

