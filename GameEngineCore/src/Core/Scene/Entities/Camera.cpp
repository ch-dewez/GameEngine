#include "Camera.h"
#include "Core/Scene/Components/Camera.h"
#include "Core/Scene/Components/Transform.h"
#include <iostream>
#include <memory>


namespace Engine {
namespace Entities {

void Camera::load() {
    auto& transform = addComponent<Components::Transform>();
    
    // Set initial camera position and orientation
    transform.position = glm::vec3(0.0f, 0.0f, 3.0f);  // Move camera back 3 units
    transform.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // Identity rotation
    transform.setForwardVector(glm::vec3{0.0f, 0.0f, -1.0f});
    transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    
    addComponent<Components::Camera>();

    tags.push_back("Main Camera");
}

}
}

