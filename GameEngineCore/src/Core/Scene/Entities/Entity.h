#pragma once
#include "Core/Scene/Scene.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Engine {

class Entity {
public:
  friend Scene;
  Entity(std::string name);
  ~Entity();

  // same hack
  template <typename T, typename, typename...> struct dependent_type {
    using type = T;
  };
  template <typename T, typename P0, typename... P>
  using dependent_type_t = typename dependent_type<T, P0, P...>::type;

  template <typename T, class... Args> T &addComponent(Args... args) {
    T &component = m_scene->addComponent<T>(args...);
    ((dependent_type_t<Components::Component &, T>)component).setEntity(this);
    return component;
  };

  template <typename T> std::vector<T &> getComponents() {
    static_assert(std::is_base_of<Components::Component, T>::value,
                  "T must inherit from Component");

    Utils::StaticArray<T> &components = m_scene->getComponents<T>();
    std::vector<T &> filtered = components.getFilteredVector([this](
                                                                 T &element) {
      return ((dependent_type_t<Components::Component &, T>)element).m_entity ==
             this;
    });

    return filtered;
  };

  template <typename T> std::optional<T*> getComponent() {
    static_assert(std::is_base_of<Components::Component, T>::value,
                  "T must inherit from Component");

    Utils::StaticArray<T> &components = m_scene->getComponents<T>();

    for (T &component : components) {
      if (((dependent_type_t<Components::Component &, T>)component).m_entity ==
          this) {
        return &component;
      }
    }
    return {};
  };

  void setScene(Scene *scene) { m_scene = scene; };

public:
  UUID uuid;
  std::string name;
  std::vector<std::string> tags;

private:
private:
  Scene *m_scene;
};

} // namespace Engine
