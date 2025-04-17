#pragma once
#include "Scene/Components/Component.h"
#include <chrono>

namespace Game {
namespace Components {

class BoxMovement : public Engine::Components::Component {
public:
    BoxMovement(std::weak_ptr<Engine::Entity> entity);
    void update() override;
    void start() override;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
};

} // namespace Components
} // namespace Game
