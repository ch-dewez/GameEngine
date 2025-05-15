#include "Camera.h"
#include <glm/glm.hpp>
#include <iostream>
#include <stdexcept>
#include "Core/Scene/Entities/Entity.h"
#include "Transform.h"

namespace Engine {
namespace Components {

glm::mat4 Camera::getViewMatrix(glm::vec3 up) {
    auto transform = m_entity->getComponent<Transform>().value().lock();

    // Calculate camera position and target
    glm::vec3 position = transform->position;
    glm::vec3 forward = transform->getForwardVector();
    glm::vec3 target = position + forward;

    
    auto view = glm::lookAt(position, target, up);
    
    return view;
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) {
    glm::mat4 proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    proj[1][1] *= -1;

    return proj;
}

}
}


