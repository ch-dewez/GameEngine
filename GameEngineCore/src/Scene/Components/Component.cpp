#include "Component.h"

namespace Engine {
namespace Components {
Component::Component(std::weak_ptr<Entity> entity) : m_entity(entity) {
}

void Component::update() {
}

void Component::start() {
}

}
}
