#include "CameraMovement.h"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <cmath>
#include <chrono>
#include <glm/gtc/constants.hpp>

namespace Game {
namespace Components {

CameraMovement::CameraMovement()
: Component()
{
}

void CameraMovement::start(){
    m_startTime = std::chrono::high_resolution_clock::now();
    m_transform = m_entity->getComponent<Engine::Components::Transform>().value_or(nullptr);
}

void CameraMovement::update(float dt) {

    Engine::Input& input = Engine::Input::Instance();
    
    // Handle mouse rotation
    glm::vec2 mouseDelta = input.getMouseDelta();
    // the first time we move the mouseDelta is way to big like 500 so I cap it at 50 (per frame so it's fine )
    mouseDelta.x = std::min(mouseDelta.x, 50.0f);
    mouseDelta.y = std::min(mouseDelta.y, 50.0f);
    mouseDelta.x = std::max(mouseDelta.x, -50.0f);
    mouseDelta.y = std::max(mouseDelta.y, -50.0f);

    float yaw = mouseDelta.x * mouseSensitivity;
    float pitch = mouseDelta.y * mouseSensitivity;

    if (std::abs(yaw) > 1e-6f) { // Check for non-zero rotation
        glm::quat yawRotation = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        m_transform->rotation = yawRotation * m_transform->rotation;
    }

    // 2. Apply Pitch rotation (around local X-axis)
    // To rotate around the local X-axis, we post-multiply by the pitch quaternion.
    if (std::abs(pitch) > 1e-6f) { // Check for non-zero rotation
        glm::quat pitchRotation = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        m_transform->rotation = m_transform->rotation * pitchRotation;
    }
    
    /*euler[1] = sin(m_angle[1]);*/
    /*euler[2] = sin(m_angle[0]) * cos(m_angle[1]);*/
    /*euler[0] = cos(m_angle[0]) * cos(m_angle[1]);*/
    /**/
    /*m_transform->rotation = glm::quat(euler);*/

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


    glm::vec3 forward = m_transform->getForwardVector();
    glm::vec3 right = m_transform->getRightVector();
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Apply movement in camera-relative directions
    if (delta != glm::vec3(0.0)) {
        m_transform->position += glm::normalize(delta.x * forward + delta.y * up + delta.z * right) * speed;
    }
}

}
}
