#include "Scene.h"
#include "Components/MeshRenderer.h"
#include "Components/Physics/Colliders.h"
#include "Components/Renderer.h"
#include "Core/Scene/Entities/Entity.h"
#include "Components/Component.h"
#include <memory>
#include <optional>

//Scene::Scene(){
//};
//
//Scene::~Scene(){
//};

namespace Engine {

Scene::Scene(){
}

void Scene::initialize(){
    initObject();
    m_isFullyInitialised = true;
    callStart();
}

void Scene::callStart(){
    auto& allComponentsArrays = m_components.getAllArrays();
    for (auto& array : allComponentsArrays){
        auto* staticArrayBase = array.second;
        Assert(staticArrayBase != nullptr, "StaticArray base pointer is null");
        auto* staticArray = static_cast<Utils::StaticArray<Components::Component>*>(staticArrayBase);
        for (auto& component : *staticArray){
            component.start();
        }
    }
};

void Scene::updateComponents(float dt) {
    auto& allComponentsArrays = m_components.getAllArrays();
    for (auto& array : allComponentsArrays){
        auto* staticArrayBase = array.second;
        Assert(staticArrayBase != nullptr, "StaticArray base pointer is null");
        auto* staticArray = static_cast<Utils::StaticArray<Components::Component>*>(staticArrayBase);
        for (auto& component : *staticArray){
            component.update(dt);
        }
    }
}


void Scene::removeEntity(UUID id) {
    m_entities.filterFirst([&](Entity& element) {return element.uuid == id;});
    auto& allComponentsArrays = m_components.getAllArrays();
    for (auto& array : allComponentsArrays){
        auto* staticArrayBase = array.second;
        Assert(staticArrayBase != nullptr, "StaticArray base pointer is null");
        auto* staticArray = static_cast<Utils::StaticArray<Components::Component>*>(staticArrayBase);
        staticArray->filter([&](Components::Component& component){return component.m_entity->uuid == id;});
    }
};

std::optional<Entity*> Scene::getEntityById(UUID id){
    auto result = m_entities.getFirstMatch([&](Entity& entity) {
        return entity.uuid == id;
    });
    
    if (result.has_value()) {
        return result.value();
    }
    return std::nullopt;
}


std::optional<Entity*> Scene::getEntityByTag(char* tag){
    auto result = m_entities.getFirstMatch([&](Entity& entity) {
        for (auto& string : entity.tags){
            if (string == tag){
                return true;
            };
        }
        return false;
    });
    
    if (result.has_value()) {
        return result.value();
    }
    return std::nullopt;
}


std::optional<Entity*> Scene::getEntityByName(char* name){
    auto result = m_entities.getFirstMatch([&](Entity& entity) {
        return entity.name == name;
    });
    
    if (result.has_value()) {
        return result.value();
    }
    return std::nullopt;
}

}






