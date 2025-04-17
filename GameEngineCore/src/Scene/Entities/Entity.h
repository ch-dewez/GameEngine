#pragma once
#include "Scene/Scene.h"
#include <memory>
#include <string>
#include <vector>

namespace Engine {

class Entity {
public:
    Entity(std::string name);

    void updateComponents();

    void addComponent(std::shared_ptr<Components::Component> component);

    template<typename T>
    std::vector<std::shared_ptr<T>> getComponents() {
        static_assert(std::is_base_of<Components::Component, T>::value, 
        "T must inherit from Component");

        std::vector<std::shared_ptr<T>> components;

        for (const auto& component : m_components) {
            if (std::shared_ptr<T> cast = std::dynamic_pointer_cast<T>(component)) {
                components.push_back(cast);
            }
        }

        return components;
    }

    template<typename T>
    std::shared_ptr<T> getComponent() {
        static_assert(std::is_base_of<Components::Component, T>::value, 
        "T must inherit from Component");

        for (const auto& component : m_components) {
            if (auto cast = std::dynamic_pointer_cast<T>(component)) {
                return cast;
            }
        }
        return nullptr;
    }
public:
    UUID uuid;
    std::string name;
    std::vector<std::string> tags;
private:

private:
    Scene* m_scene;
    std::vector<std::shared_ptr<Components::Component>> m_components;
};

} // namespace Engine
