#include <GLFW/glfw3.h>
#include <memory>
#include "Component.h"
#include "Ressources/Material.h"
#include "Renderer/Renderer.h"
#include "functional"

namespace Engine {
namespace Components {

class Renderer: public Component {
public:
    static void SetModelBufferAllocator(std::function<uint32_t()> allocator);
    static void SetModelBufferDeallocator(std::function<void(uint32_t)> deallocator);
    
    Renderer(std::shared_ptr<Ressources::Material> material); 
    virtual ~Renderer();  // Changed from =default to explicit declaration

    void update(float dt) override {};
    void start() override {};

    virtual void render(Engine::Renderer::Renderer::FrameInfo& frameInfo) = 0;
    uint32_t getModelBufferIndex() const { return m_modelBufferIndex; }
    std::weak_ptr<::Engine::Ressources::Material> getMaterial() { return m_material; }

protected:
    std::shared_ptr<::Engine::Ressources::Material> m_material;
    uint32_t m_modelBufferIndex;
    static std::function<uint32_t()> s_modelBufferAllocator;
    static std::function<void(uint32_t)> s_modelBufferDeallocator;
};

}
}
