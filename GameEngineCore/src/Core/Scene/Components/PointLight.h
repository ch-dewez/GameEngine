#include "Component.h"
#include "Core/Renderer/Lights.h"

namespace Engine {
namespace Components {

// TODO: use transform position
class PointLight: public Component {
public:
    PointLight();
    ::Engine::Renderer::PointLight lightInfo;
};

}
}
