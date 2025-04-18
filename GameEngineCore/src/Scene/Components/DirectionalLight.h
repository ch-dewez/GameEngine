#include "Component.h"
#include "Renderer/Lights.h"
#include <memory>

namespace Engine {
namespace Components {

class DirectionalLight: public Component {
public:
    DirectionalLight(std::weak_ptr<Entity> entity);
    ::Engine::Renderer::DirectionalLight lightInfo;
};

}
}
