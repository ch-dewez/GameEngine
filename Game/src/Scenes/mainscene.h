#pragma once
#include "GameEngine.h"

namespace Game {

class MainScene : public Engine::Scene {
public:
    MainScene() = default;
    ~MainScene() = default;
    void initialize() override;
};

} // namespace Game
