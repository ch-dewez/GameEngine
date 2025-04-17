#include "Component.h"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include <chrono>

namespace Engine {
namespace Components {

class Transform : public Component {
public:
    Transform(std::weak_ptr<Entity> parentEntity);

    //void update() override;
    glm::mat4 getModelMatrix();
    glm::vec3 getForwardVector();
    glm::vec3 getRightVector();
    glm::vec3 getUpVector();

    glm::vec3 setForwardVector(glm::vec3 forward, glm::vec3 up = glm::vec3{0.0, 1.0, 0.0});

public:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
};


}
}
