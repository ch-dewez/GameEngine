#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <utility>
#include "RessourceManager.h"
#include "Pipeline.h"

namespace Engine {
namespace Ressources {

static RessourceManager* instance;

void RessourceManager::Init() {
    instance = new RessourceManager();
}

RessourceManager& RessourceManager::getInstance(){
    return *instance;
}

void RessourceManager::Shutdown() {
    delete instance;
    instance = nullptr;
}

RessourceManager::RessourceManager () {
}


void RessourceManager::clearAll() {
    std::cout << "clearing all" << std::endl;
    m_textures.clear();
    m_materials.clear();
    m_pipelines.clear();
}

std::optional<std::shared_ptr<Texture>> RessourceManager::getTexture(std::string key){
    auto texture = m_textures.find(key);
    if (texture == m_textures.end()) return {};

    return texture->second;
};

std::optional<std::shared_ptr<Material>> RessourceManager::getMaterial(std::string key){
    auto material = m_materials.find(key);
    if (material == m_materials.end()) 
        return {};

    return material->second;

};

/*std::optional<std::shared_ptr<Pipeline>> RessourceManager::getPipeline(std::string key) {*/
/*    auto pipeline = m_pipelines.find(key);*/
/*    if (pipeline == m_pipelines.end()) */
/*        return {};*/
/**/
/*    return pipeline->second;*/
/*};*/

std::shared_ptr<Texture> RessourceManager::loadTexture(std::string key, std::shared_ptr<Texture> texture){
    auto existing = m_textures.find(key);
    if (existing != m_textures.end()) 
        return existing->second;

    m_textures.insert(std::pair<std::string, std::shared_ptr<Texture>>(key, texture));
    return texture;

};
std::shared_ptr<Material> RessourceManager::loadMaterial(std::string key, std::shared_ptr<Material> material){
    auto existing = m_materials.find(key);
    if (existing != m_materials.end()) 
        return existing->second;

    m_materials.insert(std::pair<std::string, std::shared_ptr<Material>>(key, material));
    return material;

};


std::shared_ptr<Texture> RessourceManager::createTexture(std::string key, const std::string &imagePath) {}

std::shared_ptr<MaterialTemplate> RessourceManager::createMaterialTemplate(std::string key, size_t materialSize, std::shared_ptr<Pipeline> pipeline) {
    auto existing = m_materialsTemplate.find(key);
    if (existing != m_materialsTemplate.end()) 
        return existing->second;

    std::shared_ptr<MaterialTemplate> matTemplate = std::make_shared<MaterialTemplate>(materialSize, pipeline);
    m_materialsTemplate.insert(std::pair<std::string, std::shared_ptr<MaterialTemplate>>(key, matTemplate));
    return matTemplate;
};

std::shared_ptr<Material> RessourceManager::createMaterial(std::string key, std::shared_ptr<MaterialTemplate> matTemplate) {
    auto existing = m_materials.find(key);
    if (existing != m_materials.end()) 
        return existing->second;

    std::shared_ptr<Material> mat = std::make_shared<Material>(matTemplate);
    m_materials.insert(std::pair<std::string, std::shared_ptr<Material>>(key, mat));
    return mat;
}


/*std::shared_ptr<Pipeline> RessourceManager::createPipeline(std::string key, const PipelineConfigInfo &configInfo) {*/
/*    auto existing = m_pipelines.find(key);*/
/*    if (existing != m_pipelines.end()) */
/*        return existing->second;*/
/**/
/*    std::shared_ptr<Pipeline> pipeline = std::make_shared<Pipeline>(configInfo);*/
/*    m_pipelines.insert(std::pair<std::string, std::shared_ptr<Pipeline>>(key, pipeline));*/
/*    return pipeline;*/
/*}*/

}
}

