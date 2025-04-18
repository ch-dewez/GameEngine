#include "Component.h"
#include "Renderer/Lights.h"

namespace Engine {
namespace Components {

class PointLight: public Component {
public:
    PointLight(std::weak_ptr<Entity> entity);
    ::Engine::Renderer::PointLight lightInfo;
};

}
}
