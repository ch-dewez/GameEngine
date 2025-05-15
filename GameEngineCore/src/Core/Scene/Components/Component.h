#pragma once
#include "Core/Scene/Scene.h"
#include "../Entities/Entity.h"

namespace Engine {
namespace Components {

class Component {
public:
    virtual void update(float dt);
    virtual void start();
    //virtual void onEvent();
    
    void setEntity(Entity* entity);
    void setScene(Scene* scene);
public:
    
    Entity* m_entity; // this one can be a pointer and not a weak_ptr because if the entity doesn't exist nor does the component
protected:
    Scene* m_scene;
};

} // namespace Components
} // namespace Engine
