#include "Renderer.h"
#include <memory>
#include "Ressources/Mesh.h"
#include "Renderer/Renderer.h"

namespace Engine {
namespace Components {

class MeshRenderer: public Renderer {
public:

    MeshRenderer(std::shared_ptr<Ressources::Material> material, std::shared_ptr<Ressources::Mesh> mesh);

    void render(Engine::Renderer::Renderer::FrameInfo& frameInfo) override;

private:
    std::shared_ptr<Ressources::Mesh> m_mesh;
};


}
}

