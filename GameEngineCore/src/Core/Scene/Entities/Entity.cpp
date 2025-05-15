#include "Entity.h"
#include "Core/Scene/Components/Component.h"

namespace Engine {

Entity::Entity(std::string name) {
    this->name = name;
    uuid.generate_uuid_v4();
}

void Entity::updateComponents(float dt) {
    for (auto component : m_components) {
        component->update(dt);
    }
}

void Entity::addComponent(std::shared_ptr<Components::Component> component){
    component->setEntity(this);
    m_components.push_back(component);
}


}
