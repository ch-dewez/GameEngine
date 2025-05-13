#pragma once
#include "Material.h"
#include "Pipeline.h"
#include "Texture.h"
#include "Mesh.h"
#include <map>
#include <memory>
#include <optional>
#include <string>

namespace Engine {
namespace Ressources {

class RessourceManager {
public:
  RessourceManager();
  static void Init();
  static RessourceManager& getInstance();
  static void Shutdown();

public:
  void clearAll();

  std::optional<std::shared_ptr<Texture>> getTexture(const std::string& key);
  std::optional<std::shared_ptr<Material>> getMaterial(const std::string& key);
  std::optional<std::shared_ptr<MaterialTemplate>> getMaterialTemplate(const std::string& key);
  //std::optional<std::shared_ptr<Pipeline>> getPipeline(const std::string& key);

  void loadTexture(const std::string& key, std::shared_ptr<Texture> texture);
  void loadMaterial(const std::string& key, std::shared_ptr<Material> material);
  void loadMaterialTemplate(const std::string& key, std::shared_ptr<MaterialTemplate> materialTemplate);
  void loadMesh(const std::string& key, std::shared_ptr<Mesh> mesh);
  //void loadPipeline(const std::string& key, std::shared_ptr<Pipeline> pipeline);

  std::shared_ptr<Texture> loadOrGetTexture(const std::string& key, std::shared_ptr<Texture> texture);
  std::shared_ptr<Material> loadOrGetMaterial(const std::string& key, std::shared_ptr<Material> material);
  std::shared_ptr<MaterialTemplate> loadOrGetMaterialTemplate(const std::string& key, std::shared_ptr<MaterialTemplate> materialTemplate);
  std::shared_ptr<Mesh> loadOrGetMesh(const std::string& key, std::shared_ptr<Mesh> mesh);
  //std::shared_ptr<Pipeline> loadOrGetPipeline(const std::string& key, std::shared_ptr<Pipeline> pipeline);

  // Create methods
  /*std::shared_ptr<Texture> createTexture(const std::string& key, const std::string &imagePath);*/
  /*std::shared_ptr<Material> createMaterial(const std::string& key, size_t matSize, std::shared_ptr<MaterialTemplate> matTemplate);*/
  /*std::shared_ptr<MaterialTemplate> createMaterialTemplate(const std::string& key, std::shared_ptr<Pipeline> pipeline);*/
  //std::shared_ptr<Pipeline> createPipeline(std::string key, const PipelineConfigInfo &configInfo);

private:
  std::map<std::string, std::shared_ptr<Mesh>> m_meshes;
  std::map<std::string, std::shared_ptr<Texture>> m_textures;
  std::map<std::string, std::shared_ptr<Material>> m_materials;
  std::map<std::string, std::shared_ptr<MaterialTemplate>> m_materialsTemplate;
  std::map<std::string, std::shared_ptr<Pipeline>> m_pipelines;
};

} // namespace Ressources
} // namespace Engine
