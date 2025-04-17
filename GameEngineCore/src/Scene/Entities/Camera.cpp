#include "Camera.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/Transform.h"
#include <iostream>
#include <memory>


namespace Engine {
namespace Entities {

void Camera::load(std::shared_ptr<Entity> self) {
    std::shared_ptr<Components::Transform> transform = std::make_shared<Components::Transform>(std::weak_ptr<Entity>(self));
    
    // Set initial camera position and orientation
    transform->position = glm::vec3(0.0f, 0.0f, 3.0f);  // Move camera back 3 units
    transform->rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // Identity rotation
    transform->setForwardVector(glm::vec3{0.0f, 0.0f, -1.0f});
    transform->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    
    addComponent(transform);
    std::cout << transform->getForwardVector().y << std::endl;
    std::cout << transform->getForwardVector().z << std::endl;
    std::cout << transform->getForwardVector().x << std::endl;

    std::shared_ptr<Components::Camera> cameraComponent = std::make_shared<Components::Camera>(std::weak_ptr<Entity>(self));
    addComponent(cameraComponent);

    self->tags.push_back("Main Camera");
}

}
}

