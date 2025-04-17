#include "BoxMovement.h"
#include "Scene/Components/Transform.h"
#include "Scene/Entities/Entity.h"
#include <iostream>
#include <cmath>
#include <chrono>

namespace Game {
namespace Components {

BoxMovement::BoxMovement(std::weak_ptr<Engine::Entity> entity)
: Component(entity), startTime(std::chrono::high_resolution_clock::now()) {
}

void BoxMovement::update() {
    if (m_entity.expired()) {
        return;
    }
    
    auto entity = m_entity.lock();
    auto transform = entity->getComponent<Engine::Components::Transform>();
    if (!transform) {
        return;
    }


    // Calculate elapsed time in seconds
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float>(currentTime - startTime).count();

    // Oscillate between -2 and 2 on the x-axis
    float amplitude = 2.0f;
    float frequency = 1.0f; // One complete oscillation per second
    transform->position.x = amplitude * std::sin(frequency * time);
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
