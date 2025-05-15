#include "Component.h"
#include "Core/Renderer/Lights.h"

namespace Engine {
namespace Components {

class PointLight: public Component {
public:
    PointLight();
    ::Engine::Renderer::PointLight lightInfo;
};

}
}
