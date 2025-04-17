#include "Entity.h"
#include "Scene/Components/Component.h"

namespace Engine {

Entity::Entity(std::string name) {
    this->name = name;
    uuid.generate_uuid_v4();
}

void Entity::updateComponents() {
    for (auto component : m_components) {
        component->update();
    }
}

void Entity::addComponent(std::shared_ptr<Components::Component> component){
    m_components.push_back(component);
}


}
