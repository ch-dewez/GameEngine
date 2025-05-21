#include "Renderer.h"
#include "Core/Ressources/DescriptorsManager.h"
#include "Core/Log/Log.h"

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

Renderer::Renderer(std::shared_ptr<Ressources::Material> material)
: Component(), m_material(material)
{
    Assert(s_modelBufferAllocator, "Model buffer allocator not set. Call Renderer::SetModelBufferAllocator first.");
    m_modelBufferIndex = s_modelBufferAllocator();
}

Renderer::~Renderer() {
    LogDebug("calling renderer deconstructor");
    s_modelBufferDeallocator(m_modelBufferIndex);
}

}
}



