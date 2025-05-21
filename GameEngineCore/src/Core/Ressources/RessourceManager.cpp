#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <utility>
#include "RessourceManager.h"
#include "Material.h"
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

RessourceManager::~RessourceManager(){
    clearAll();
}


void RessourceManager::clearAll() {
    LogDebug("clear all ressources");
    for (auto& mesh : m_meshes){
        LogDebug(mesh.second.use_count());
    }
    m_meshes.clear();
    m_textures.clear();
    m_materials.clear();
    m_materialsTemplate.clear();
    m_pipelines.clear();
}

std::optional<std::shared_ptr<Texture>> RessourceManager::getTexture(const std::string& key){
    auto texture = m_textures.find(key);
    if (texture == m_textures.end()) return {};

    return texture->second;
};

std::optional<std::shared_ptr<Material>> RessourceManager::getMaterial(const std::string& key){
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

void RessourceManager::loadTexture(const std::string& key, std::shared_ptr<Texture> texture) {
    auto existing = m_textures.find(key);
    if (existing != m_textures.end()) {
        throw std::runtime_error("Texture with key '" + key + "' already exists");
    }
    m_textures.insert(std::pair<std::string, std::shared_ptr<Texture>>(key, texture));
}


std::shared_ptr<Texture> RessourceManager::loadOrGetTexture(const std::string& key, std::shared_ptr<Texture> texture) {
    auto existing = m_textures.find(key);
    if (existing != m_textures.end()) {
        return existing->second;
    }
    m_textures.insert(std::pair<std::string, std::shared_ptr<Texture>>(key, texture));
    return texture;
}


std::shared_ptr<Material> RessourceManager::loadOrGetMaterial(const std::string& key, std::shared_ptr<Material> material) {
    auto existing = m_materials.find(key);
    if (existing != m_materials.end()) {
        return existing->second;
    }
    m_materials.insert(std::pair<std::string, std::shared_ptr<Material>>(key, material));
    return material;
}

void RessourceManager::loadMaterial(const std::string& key, std::shared_ptr<Material> material) {
    auto existing = m_materials.find(key);
    if (existing != m_materials.end()) {
        throw std::runtime_error("Material with key '" + key + "' already exists");
    }
    m_materials.insert(std::pair<std::string, std::shared_ptr<Material>>(key, material));
}


std::shared_ptr<MaterialTemplate> RessourceManager::loadOrGetMaterialTemplate(const std::string& key, std::shared_ptr<MaterialTemplate> materialTemplate) {
    auto existing = m_materialsTemplate.find(key);
    if (existing != m_materialsTemplate.end()) {
        return existing->second;
    }
    m_materialsTemplate.insert(std::pair<std::string, std::shared_ptr<MaterialTemplate>>(key, materialTemplate));
    return materialTemplate;
}


void RessourceManager::loadMaterialTemplate(const std::string& key, std::shared_ptr<MaterialTemplate> materialTemplate) {
    auto existing = m_materialsTemplate.find(key);
    if (existing != m_materialsTemplate.end()) {
        throw std::runtime_error("MaterialTemplate with key '" + key + "' already exists");
    }
    m_materialsTemplate.insert(std::pair<std::string, std::shared_ptr<MaterialTemplate>>(key, materialTemplate));
}

void RessourceManager::loadMesh(const std::string& key, std::shared_ptr<Mesh> mesh){
    auto existing = m_meshes.find(key);
    if (existing != m_meshes.end()) {
        throw std::runtime_error("MaterialTemplate with key '" + key + "' already exists");
    }
    m_meshes.insert(std::pair<std::string, std::shared_ptr<Mesh>>(key, mesh));
};

std::shared_ptr<Mesh> RessourceManager::loadOrGetMesh(const std::string& key, std::shared_ptr<Mesh> mesh){

    auto existing = m_meshes.find(key);
    if (existing != m_meshes.end()) {
        return existing->second;
    }
    m_meshes.insert(std::pair<std::string, std::shared_ptr<Mesh>>(key, mesh));
    return mesh;
};




/*std::shared_ptr<Texture> RessourceManager::createTexture(const std::string& key, const std::string &imagePath) {*/
/*    auto existing = m_textures.find(key);*/
/*    if (existing != m_textures.end()) */
/*        return existing->second;*/
/**/
/*    std::shared_ptr<Texture> texture = std::make_shared<Texture>(imagePath);*/
/*    m_textures.insert(std::pair<std::string, std::shared_ptr<Texture>>(key, texture));*/
/*    return texture;*/
/*};*/
/**/
/*std::shared_ptr<MaterialTemplate> RessourceManager::createMaterialTemplate(const std::string& key, std::shared_ptr<Pipeline> pipeline) {*/
/*    auto existing = m_materialsTemplate.find(key);*/
/*    if (existing != m_materialsTemplate.end()) */
/*        return existing->second;*/
/**/
/*    std::shared_ptr<MaterialTemplate> matTemplate = std::make_shared<MaterialTemplate>(pipeline);*/
/*    m_materialsTemplate.insert(std::pair<std::string, std::shared_ptr<MaterialTemplate>>(key, matTemplate));*/
/*    return matTemplate;*/
/*};*/
/**/
/*std::shared_ptr<Material> RessourceManager::createMaterial(const std::string& key, size_t matSize, std::shared_ptr<MaterialTemplate> matTemplate) {*/
/*    auto existing = m_materials.find(key);*/
/*    if (existing != m_materials.end()) */
/*        return existing->second;*/
/**/
/*    std::shared_ptr<Material> mat = std::make_shared<Material>(matTemplate, matSize);*/
/*    m_materials.insert(std::pair<std::string, std::shared_ptr<Material>>(key, mat));*/
/*    return mat;*/
/*}*/


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

