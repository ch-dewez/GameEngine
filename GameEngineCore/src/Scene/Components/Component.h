#pragma once
#include "Scene/Scene.h"
#include <memory>

namespace Engine {
namespace Components {

class Component {
public:
    Component(std::weak_ptr<Entity> parentEntity);

    virtual void update();
    virtual void start();
    //virtual void onEvent();
    
    
public:
    
protected:
    std::weak_ptr<Entity> m_entity;
};

} // namespace Components
} // namespace Engine
