#include "Transform.h"
#include "Component.h"
#include <chrono>

namespace Engine {
namespace Components {

Transform::Transform(std::weak_ptr<Entity> parentEntity)
: Component(parentEntity), position(0.0,0.0,0.0), rotation(1.0f, 0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f)
{
}

glm::mat4 Transform::getModelMatrix() {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    
    // First translate
    modelMatrix = glm::translate(modelMatrix, position);
    
    // Then rotate using the quaternion
    modelMatrix = modelMatrix * glm::mat4_cast(rotation);
    
    // Finally scale
    modelMatrix = glm::scale(modelMatrix, scale);
    
    return modelMatrix;
}

glm::vec3 Transform::getForwardVector() {
    return glm::rotate(glm::inverse(rotation), glm::vec3(0.0, 0.0, -1.0));
}


glm::vec3 Transform::getRightVector() {
    return glm::rotate(glm::inverse(rotation), glm::vec3(1.0, 0.0, 0.0));
}

glm::vec3 Transform::getUpVector() {
    return glm::rotate(glm::inverse(rotation), glm::vec3(0.0, 1.0, 0.0));
}

glm::vec3 Transform::setForwardVector(glm::vec3 forward, glm::vec3 up) {
    forward = glm::normalize(forward);
    up = glm::normalize(up);
    
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    up = glm::cross(right, forward);
    
    glm::mat3 rotationMatrix(right, up, -forward);
    rotation = glm::quat_cast(rotationMatrix);
    
    return forward;
}

}
}

