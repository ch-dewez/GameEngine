#pragma once
#include "Material.h"
#include "Pipeline.h"
#include "Texture.h"
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

  std::optional<std::shared_ptr<Texture>> getTexture(std::string key);
  std::optional<std::shared_ptr<Material>> getMaterial(std::string key);
  std::optional<std::shared_ptr<MaterialTemplate>> getMaterialTemplate(std::string key);
  //std::optional<std::shared_ptr<Pipeline>> getPipeline(std::string key);

  std::shared_ptr<Texture> loadTexture(std::string key, std::shared_ptr<Texture> texture);
  std::shared_ptr<Material> loadMaterial(std::string key, std::shared_ptr<Material> material);
  std::shared_ptr<MaterialTemplate> loadMaterialTemplate(std::string key, std::shared_ptr<MaterialTemplate> materialTemplate);
  //std::shared_ptr<Pipeline> loadPipeline(std::string key, std::shared_ptr<Pipeline> pipeline);

  // Create methods
  std::shared_ptr<Texture> createTexture(std::string key, const std::string &imagePath);
  std::shared_ptr<Material> createMaterial(std::string key, std::shared_ptr<MaterialTemplate> matTemplate);
  std::shared_ptr<MaterialTemplate> createMaterialTemplate(std::string key, size_t materialSize, std::shared_ptr<Pipeline> pipeline);
  //std::shared_ptr<Pipeline> createPipeline(std::string key, const PipelineConfigInfo &configInfo);

private:
  std::map<std::string, std::shared_ptr<Texture>> m_textures;
  std::map<std::string, std::shared_ptr<Material>> m_materials;
  std::map<std::string, std::shared_ptr<MaterialTemplate>> m_materialsTemplate;
  std::map<std::string, std::shared_ptr<Pipeline>> m_pipelines;
};

} // namespace Ressources
} // namespace Engine
