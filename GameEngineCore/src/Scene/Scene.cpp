#include "Scene.h"
#include "Scene/Entities/Entity.h"
#include "Components/Component.h"
#include <memory>
#include <optional>

//Scene::Scene(){
//};
//
//Scene::~Scene(){
//};

namespace Engine {
void Scene::updateComponents() {
    for (auto entity : m_entities) {
        entity->updateComponents();
    }
}

void Scene::addEntity(std::shared_ptr<Entity> entity) {
    m_entities.push_back(entity);
};

void Scene::removeEntity(UUID id) {
    for (int i=0;i<m_entities.size();i++) {
        std::shared_ptr<Entity> entity = m_entities[i];
        if (entity->uuid == id) {
            m_entities.erase(m_entities.begin() + i);
        }
    }
};

std::optional<std::weak_ptr<Entity>> Scene::getEntityById(UUID id){
    for (int i=0;i<m_entities.size();i++) {
        std::shared_ptr<Entity> entity = m_entities[i];
        if (entity->uuid == id) {
            return entity; 
        }
    }
    return {};
};


std::optional<std::weak_ptr<Entity>> Scene::getEntityByTag(char* tag){
    for (int i=0;i<m_entities.size();i++) {
        std::shared_ptr<Entity> entity = m_entities[i];
        for (int j=0;j<entity->tags.size();j++) {
            const std::string& entityTag = entity->tags[j];
            if (tag == entityTag) {
                return entity; 
            }
        }
    }
    
    return {};
};


std::optional<std::weak_ptr<Entity>> Scene::getEntityByName(char* name){
    for (int i=0;i<m_entities.size();i++) {
        std::shared_ptr<Entity> entity = m_entities[i];
        if (entity->name == name) {
            return entity; 
        }
    }

    return {};
};

}




