#include "Component.h"
#include "Renderer/Lights.h"
#include <memory>

namespace Engine {
namespace Components {

class DirectionalLight: public Component {
public:
    DirectionalLight();
    ::Engine::Renderer::DirectionalLight lightInfo;
};

}
}
