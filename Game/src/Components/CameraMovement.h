#pragma once
#include "GameEngine.h"
#include <chrono>
#include <glm/glm.hpp>

namespace Game {
namespace Components {

class CameraMovement : public Engine::Components::Component {
public:
    CameraMovement();
    void update(float dt) override;
    void start() override;

    float speed = 0.2;
    float mouseSensitivity = 0.002f;
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
    Engine::Components::Transform* m_transform = nullptr;
};

} // namespace Components
} // namespace Game
