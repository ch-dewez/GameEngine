#pragma once
#include <optional>
#include "Core/UUID.h"
#include "Core/Utils/StaticArray.h"
#include <memory>
#include <vector>

namespace Engine {

class Entity;
namespace Components{
class Component;
}

class Scene {
    friend Entity;
public:
    Scene();
    virtual ~Scene() = default;

    void updateComponents(float dt);

    // what the heck ??
    // this is a hack because template need to be in header file but can't import Entity.h because circular dependency and can't use forward declaration because of incomplete type
    template <typename T, typename, typename...>
    struct dependent_type {using type = T;};
    template <typename T, typename P0, typename ...P>
    using dependent_type_t = typename dependent_type<T, P0, P...>::type;

    template<typename T = Entity, class... Args>
    T& addEntity(Args... args){
        static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Engine::Entity");
        T& entity = (T&)m_entities.add_emplace<T>(args...);
        ((dependent_type_t<Entity, T>&)entity).setScene(this);
        return entity;
    };
    void removeEntity(UUID id);

    Utils::StaticArray<Entity>& getAllEntities() { return m_entities;};

    std::optional<Entity*> getEntityById(UUID id);
    std::optional<Entity*> getEntityByTag(char* tag);
    std::optional<Entity*> getEntityByName(char* name);

    template<typename T>
    Utils::StaticArray<T>& getComponents(){
        return m_components.getArray<T>();
    };
    Utils::StaticArrayRegistry<Engine::Components::Component>& getComponentsRigistry() {return m_components;};

    void initialize();
public:
protected:
    virtual void initObject() = 0;

private:
    // need  to go through entity to add component
    template<typename T, class... Args>
    T& addComponent(Args... args){
        static_assert(std::is_base_of<Engine::Components::Component, T>::value, "T must be derived from Engine::Components::Component");
        T& component = m_components.getArray<T>().add_emplace(args...);
        ((dependent_type_t<Components::Component, T>&)component).setScene(this);
        if (m_isFullyInitialised){
            ((dependent_type_t<Components::Component, T>&)component).start();
        }
        return component;
    };

    void callStart();

private:
    Utils::StaticArray<Entity> m_entities;
    Utils::StaticArrayRegistry<Engine::Components::Component> m_components;

    bool m_isFullyInitialised;
};

} // namespace Engine
