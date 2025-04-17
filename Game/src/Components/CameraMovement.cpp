#include "CameraMovement.h"
#include "Input.h"
#include "Scene/Components/Transform.h"
#include "Scene/Entities/Entity.h"
#include <iostream>
#include <cmath>
#include <chrono>
#include <glm/gtc/constants.hpp>

namespace Game {
namespace Components {

CameraMovement::CameraMovement(std::weak_ptr<Engine::Entity> entity)
: Component(entity), startTime(std::chrono::high_resolution_clock::now()) {
}

void CameraMovement::update() {
    if (m_entity.expired()) {
        return;
    }
    
    auto entity = m_entity.lock();
    auto transform = entity->getComponent<Engine::Components::Transform>();
    if (!transform) {
        return;
    }

    Engine::Input& input = Engine::Input::Instance();
    
    // Handle mouse rotation
    glm::vec2 mouseDelta = input.getMouseDelta();
    yaw += mouseDelta.x * -1 * mouseSensitivity;
    pitch -= mouseDelta.y * -1 * mouseSensitivity; // Inverted Y for natural camera feel
    
    // Clamp pitch to avoid camera flipping
    pitch = glm::clamp(pitch, -glm::half_pi<float>() * 0.99f, glm::half_pi<float>() * 0.99f);
    
    // Calculate new forward direction
    glm::vec3 forward;

    //forward.x = self.angles[1].sin();
    forward.x += sin(yaw);
    forward.y += sin(pitch) * cos(yaw);
    forward.z += cos(pitch) * cos(yaw);

    std::cout << "forward" << std::endl;
    std::cout << forward.x << " ";
    std::cout << forward.y << " ";
    std::cout << forward.z << std::endl;
    
    //forward = transform->getForwardVector();
    // Update camera orientation
    transform->setForwardVector(forward);

    // Handle movement
    glm::vec3 delta = glm::vec3(0.0);

    if (input.isKeyPressed(GLFW_KEY_W)){
        delta.x += 1.0;
    }
    if (input.isKeyPressed(GLFW_KEY_S)){
        delta.x -= 1.0;
    }
    if (input.isKeyPressed(GLFW_KEY_A)){
        delta.z -= 1.0;
    }
    if (input.isKeyPressed(GLFW_KEY_D)){
        delta.z += 1.0;
    }
    if (input.isKeyPressed(GLFW_KEY_SPACE)){
        delta.y += 1.0;
    }
    if (input.isKeyPressed(GLFW_KEY_LEFT_SHIFT)){
        delta.y -= 1.0;
    }

    // Get the camera's orientation vectors

        /*let right = forward.cross(Vec3::new(0.0, 1.0, 0.0));*/
        /*let up = right.cross(forward);*/

    glm::vec3 right = glm::cross(forward, glm::vec3(0.0, 1.0, 0.0));
    glm::vec3 up = glm::cross(right, forward);
    /*glm::vec3 right = transform->getRightVector();*/
    /*glm::vec3 up = transform->getUpVector();*/


    std::cout << "forward 2" << std::endl;
    std::cout << forward.x << " ";
    std::cout << forward.y << " ";
    std::cout << forward.z << std::endl;

    // Apply movement in camera-relative directions
    if (delta != glm::vec3(0.0)) {
        transform->position += glm::normalize(delta.x * forward + delta.y * up + delta.z * right) * speed;
    }
}

void CameraMovement::start() {
    startTime = std::chrono::high_resolution_clock::now();
}

}
}
