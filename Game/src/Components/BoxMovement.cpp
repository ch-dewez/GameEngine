#include "BoxMovement.h"
#include "Scene/Components/Transform.h"
#include "Scene/Entities/Entity.h"
#include <iostream>
#include <cmath>
#include <chrono>

namespace Game {
namespace Components {

BoxMovement::BoxMovement()
: Component(), startTime(std::chrono::high_resolution_clock::now()) {
}

void BoxMovement::update(float dt) {
    auto transform = m_entity->getComponent<Engine::Components::Transform>().value().lock();
    // Calculate elapsed time in seconds
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float>(currentTime - startTime).count();

    // Oscillate between -2 and 2 on the x-axis
    float frequency = 1.0f; // One complete oscillation per second
    transform->position.y = amplitude * std::sin(frequency * time);
    transform->position.y -= 3.0;
    //transform->position.y = amplitude * std::sin(frequency * time);
    //transform->position.z = amplitude * std::sin(frequency * time);
    
    // Add rotation around the Y axis
    float rotationSpeed = 1.0f; // One full rotation per second
    //transform->rotation = glm::angleAxis(time * rotationSpeed * glm::pi<float>() * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void BoxMovement::start() {
    startTime = std::chrono::high_resolution_clock::now();
}

}
}
