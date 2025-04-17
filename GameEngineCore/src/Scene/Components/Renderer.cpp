#include "Renderer.h"
#include "Ressources/DescriptorsManager.h"
#include "Scene/Entities/Entity.h"
#include <stdexcept>

namespace Engine{
namespace Components {

std::function<uint32_t()> Renderer::s_modelBufferAllocator;
std::function<void(uint32_t)> Renderer::s_modelBufferDeallocator;

void Renderer::SetModelBufferAllocator(std::function<uint32_t()> allocator) {
    s_modelBufferAllocator = allocator;
}

void Renderer::SetModelBufferDeallocator(std::function<void(uint32_t)> deallocator) {
    s_modelBufferDeallocator = deallocator;
}

Renderer::Renderer(std::weak_ptr<Engine::Entity> entity, std::shared_ptr<Ressources::Material> material)
: Component(entity), m_material(material)
{
    if (!s_modelBufferAllocator) {
        throw std::runtime_error("Model buffer allocator not set. Call Renderer::SetModelBufferAllocator first.");
    }
    m_modelBufferIndex = s_modelBufferAllocator();
}

Renderer::~Renderer() {
    if (s_modelBufferDeallocator) {
        s_modelBufferDeallocator(m_modelBufferIndex);
    }
}
}
}



