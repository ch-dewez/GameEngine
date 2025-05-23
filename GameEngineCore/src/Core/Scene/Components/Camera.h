#pragma once
#include "Component.h"
#include "glm/glm.hpp"

namespace Engine {
namespace Components {

class Camera : public Component {
public:
    glm::mat4 getViewMatrix(glm::vec3 up = glm::vec3{0.0, 1.0, 0.0});
    glm::mat4 getProjectionMatrix(float aspectRatio);

    float fov = 60.0;
    float nearPlane = 0.1;
    float farPlane = 1000.0;
};


}
}

