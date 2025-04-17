#pragma once
#include <optional>
#include "UUID.h"
#include <memory>
#include <vector>

namespace Engine {

class Entity;
namespace Components{
class Component;
}

class Scene {
public:
    Scene() = default;
    virtual ~Scene() = default;

    void updateComponents();
    void render();

    void addEntity(std::shared_ptr<Entity> entity);
    void removeEntity(UUID id);

//    void addComponent();

    std::vector<std::shared_ptr<Entity>>& getAllEntities() { return m_entities;};

    std::optional<std::weak_ptr<Entity>> getEntityById(UUID id);
    std::optional<std::weak_ptr<Entity>> getEntityByTag(char* tag);
    std::optional<std::weak_ptr<Entity>> getEntityByName(char* name);

    virtual void initialize() = 0;
public:
    
private:

private:
    std::vector<std::shared_ptr<Entity> > m_entities;
};

} // namespace Engine
