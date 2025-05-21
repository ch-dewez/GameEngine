#pragma once
#include "GameEngine.h"

namespace Game {

class MainScene : public Engine::Scene {
public:
    MainScene() = default;
    ~MainScene() = default;
    void initObject() override;
};

} // namespace Game
