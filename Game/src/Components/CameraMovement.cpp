#include "CameraMovement.h"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <iostream>
#include <cmath>
#include <chrono>
#include <glm/gtc/constants.hpp>

namespace Game {
namespace Components {

CameraMovement::CameraMovement()
: Component(), m_startTime(std::chrono::high_resolution_clock::now()) {
}

void CameraMovement::update(float dt) {
    auto transform = m_entity->getComponent<Engine::Components::Transform>().value().lock();

    Engine::Input& input = Engine::Input::Instance();
    
    // Handle mouse rotation
    glm::vec2 mouseDelta = input.getMouseDelta();
    float yaw = mouseDelta.x * mouseSensitivity;
    float pitch = mouseDelta.y * mouseSensitivity;

    if (std::abs(yaw) > 1e-6f) { // Check for non-zero rotation
        glm::quat yawRotation = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        transform->rotation = yawRotation * transform->rotation;
    }

    // 2. Apply Pitch rotation (around local X-axis)
    // To rotate around the local X-axis, we post-multiply by the pitch quaternion.
    if (std::abs(pitch) > 1e-6f) { // Check for non-zero rotation
        glm::quat pitchRotation = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        transform->rotation = transform->rotation * pitchRotation;
    }
    
    /*euler[1] = sin(m_angle[1]);*/
    /*euler[2] = sin(m_angle[0]) * cos(m_angle[1]);*/
    /*euler[0] = cos(m_angle[0]) * cos(m_angle[1]);*/
    /**/
    /*transform->rotation = glm::quat(euler);*/

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


    glm::vec3 forward = transform->getForwardVector();
    glm::vec3 right = transform->getRightVector();
    glm::vec3 up = transform->getUpVector();

    // Apply movement in camera-relative directions
    if (delta != glm::vec3(0.0)) {
        transform->position += glm::normalize(delta.x * forward + delta.y * up + delta.z * right) * speed;
    }
}

void CameraMovement::start() {
    m_startTime = std::chrono::high_resolution_clock::now();
}

}
}
