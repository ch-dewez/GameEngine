#pragma once
#include "Scene/Components/Component.h"
#include <chrono>

namespace Game {
namespace Components {

class BoxMovement : public Engine::Components::Component {
public:
    BoxMovement();
    void update(float dt) override;
    void start() override;

    float amplitude = 4.0f;
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
};

} // namespace Components
} // namespace Game
