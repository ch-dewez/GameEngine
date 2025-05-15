#include "Component.h"

namespace Engine {
namespace Components {

void Component::update(float dt) {
}

void Component::start() {
}

void Component::setEntity(Entity* entity) {
    m_entity = entity;
}

void Component::setScene(Scene* scene) {
    m_scene = scene;
}

}
}
