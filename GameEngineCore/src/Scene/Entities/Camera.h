#include "Prefab.h"
#include "glm/glm.hpp"
#include <memory>

namespace Engine {
namespace Entities {

class Camera : public Prefab {
public:
    Camera(std::string name): Prefab(name) {};
    void load(std::shared_ptr<Entity> self) override;
};

}
}
