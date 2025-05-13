#include "Transform.h"
#include "Component.h"
#include <chrono>

namespace Engine {
namespace Components {

Transform::Transform()
: Component(), position(0.0,0.0,0.0), rotation(1.0f, 0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f)
{
}

glm::mat4 Transform::getModelMatrix() {

    glm::mat4 modelMatrix = getTranslationMatrix() * glm::mat4(getRotationMatrix()) * glm::mat4(getScalingMatrix());
    
    return modelMatrix;
}

glm::mat4 Transform::getTranslationMatrix(){
    return glm::translate(glm::mat4(1.0f), position);
};

glm::mat3 Transform::getRotationMatrix(){
    return glm::mat3_cast(rotation);
};

glm::mat3 Transform::getScalingMatrix(){
    return glm::scale(glm::mat4(1.0f), scale);
};


glm::vec3 Transform::transform(glm::vec3 point){
    return getModelMatrix() * glm::vec4(point, 1.0);
};

glm::vec3 Transform::inverseTransform(glm::vec3 point){
    return glm::inverse(getModelMatrix())* glm::vec4(point, 1.0);
};

glm::vec3 Transform::getForwardVector() {
    return rotation * glm::vec3(0.0, 0.0, -1.0);
    //return glm::rotate(glm::inverse(rotation), glm::vec3(0.0, 0.0, -1.0));
}


glm::vec3 Transform::getRightVector() {
    return rotation * glm::vec3(1.0, 0.0, .0);
    //return glm::rotate(glm::inverse(rotation), glm::vec3(1.0, 0.0, 0.0));
}

glm::vec3 Transform::getUpVector() {
    return rotation * glm::vec3(0.0, 1.0, .0);
    //return glm::rotate(glm::inverse(rotation), glm::vec3(0.0, 1.0, 0.0));
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

