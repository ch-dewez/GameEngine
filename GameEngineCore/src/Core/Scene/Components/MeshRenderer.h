#pragma once
#include "Renderer.h"
#include <memory>
#include "Core/Ressources/Mesh.h"
#include "Core/Renderer/Renderer.h"

namespace Engine {
namespace Components {

class MeshRenderer: public Renderer {
public:
    MeshRenderer(std::shared_ptr<Ressources::Material> material, std::shared_ptr<Ressources::Mesh> mesh);
    ~MeshRenderer();

    void render(Engine::Renderer::Renderer::FrameInfo& frameInfo) override;

private:
    std::shared_ptr<Ressources::Mesh> m_mesh;
};


}
}

