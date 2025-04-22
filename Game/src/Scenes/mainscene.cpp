#include "mainscene.h"
#include "Scene/Entities/Entity.h"
#include "Scene/Entities/Camera.h"
#include "../Components/CameraMovement.h"
#include "../Components/BoxMovement.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/MeshRenderer.h"
#include "Ressources/RessourceManager.h"
#include "Ressources/Pipeline.h"
#include "Ressources/Vertex.h"
#include "Ressources/vertexBuffer.h"
#include "Ressources/IndexBuffer.h"
#include "Ressources/Material.h"
#include "../Materials/defaultMaterial.h"
#include <memory>
#include <vector>
#include "Scene/Components/DirectionalLight.h"
#include "Scene/Components/PointLight.h"
#include "vulkan/vulkan_core.h"

namespace Game {

const std::vector<Material::PosNormalTexCoordVertex> verticesTexCoord = {
    // Front face (normal: 0, 0, 1)
    {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},   // 0
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},   // 1
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},   // 2
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},   // 3

    // Back face (normal: 0, 0, -1)
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},  // 4
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},  // 5
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},  // 6
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},  // 7

    // Left face (normal: -1, 0, 0)
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // 8
    {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},  // 9
    {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // 10
    {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},  // 11

    // Right face (normal: 1, 0, 0)
    {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},   // 12
    {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},   // 13
    {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},   // 14
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},   // 15

    // Top face (normal: 0, 1, 0)
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},   // 16
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},   // 17
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},   // 18
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},   // 19

    // Bottom face (normal: 0, -1, 0)
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},  // 20
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},  // 21
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},  // 22
    {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}   // 23
};

const std::vector<Material::PosNormalVertex> verticesColor = {
    // Front face (normal: 0, 0, 1)
    {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},   // 0
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},   // 1
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},   // 2
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},   // 3

    // Back face (normal: 0, 0, -1)
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},  // 4
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},  // 5
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},  // 6
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},  // 7

    // Left face (normal: -1, 0, 0)
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},  // 8
    {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}},  // 9
    {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}},  // 10
    {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},  // 11

    // Right face (normal: 1, 0, 0)
    {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},   // 12
    {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},   // 13
    {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},   // 14
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},   // 15

    // Top face (normal: 0, 1, 0)
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},   // 16
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},   // 17
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},   // 18
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},   // 19

    // Bottom face (normal: 0, -1, 0)
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},  // 20
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},  // 21
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}},  // 22
    {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}}   // 23
};


/*const std::vector<Vertex> vertices = {*/
/*    // Front face*/
/*    {{-0.5f, -0.5f, 0.5f}},  // 0*/
/*    {{0.5f, -0.5f, 0.5f}},   // 1*/
/*    {{0.5f, 0.5f, 0.5f}},    // 2*/
/*    {{-0.5f, 0.5f, 0.5f}},   // 3*/
/**/
/*    // Back face*/
/*    {{-0.5f, -0.5f, -0.5f}}, // 4*/
/*    {{0.5f, -0.5f, -0.5f}},  // 5*/
/*    {{0.5f, 0.5f, -0.5f}},   // 6*/
/*    {{-0.5f, 0.5f, -0.5f}}   // 7*/
/*};*/

/*const std::vector<uint16_t> indices = {*/
/*    // Front face (CCW from front)*/
/*    0, 2, 1,    0, 3, 2,*/
/*    // Back face (CCW from back)*/
/*    4, 6, 5,    4, 7, 6,*/
/*    // Left face (CCW from left)*/
/*    8, 10, 9,   8, 11, 10,*/
/*    // Right face (CCW from right)*/
/*    12, 14, 13, 12, 15, 14,*/
/*    // Top face (CCW from top)*/
/*    16, 18, 17, 16, 19, 18,*/
/*    // Bottom face (CCW from bottom)*/
/*    20, 22, 21, 20, 23, 22*/
/*};*/

/*const std::vector<uint16_t> indices = {*/
/*    // Front face*/
/*    0, 1, 2,    2, 3, 0,*/
/*    // Back face*/
/*    4, 5, 6,    6, 7, 4,*/
/*    // Left face*/
/*    8, 9, 10,   10, 11, 8,*/
/*    // Right face*/
/*    12, 13, 14, 14, 15, 12,*/
/*    // Top face*/
/*    16, 17, 18, 18, 19, 16,*/
/*    // Bottom face*/
/*    20, 21, 22, 22, 23, 20*/
/*};*/

const std::vector<uint16_t> indices = {
    // Front face (CCW from front)
    0, 1, 2,    2, 3, 0,
    // Back face (CCW from back)
    4, 6, 5,    4, 7, 6,
    // Left face (CCW from left)
    8, 9, 10,   10, 11, 8,
    // Right face (CCW from right)
    12, 13, 14, 14, 15, 12,
    // Top face (CCW from top)
    16, 17, 18, 18, 19, 16,
    // Bottom face (CCW from bottom)
    20, 21, 22, 22, 23, 20
};


void MainScene::initialize(){
    //std::shared_ptr<Engine::Entities::Camera> camera();

    std::shared_ptr<Engine::Entity> entity (new Engine::Entity("First entity"));
    std::weak_ptr<Engine::Entity> entityWeak (entity);


    auto blinnPhongColorPipelineConfig = Engine::Ressources::PipelineConfigInfo::defaultPipelineConfigInfo("shaders/GameEngineCore/blinnPhongVertColor.glsl.spv", "shaders/GameEngineCore/blinnPhongFragColor.glsl.spv", Material::PosNormalVertex::getBindingDescription(), Material::PosNormalVertex::getAttributeDescriptions());
    auto blinnPhongTexPipelineConfig = Engine::Ressources::PipelineConfigInfo::defaultPipelineConfigInfo("shaders/GameEngineCore/blinnPhongVertText.glsl.spv", "shaders/GameEngineCore/blinnPhongFragText.glsl.spv", Material::PosNormalTexCoordVertex::getBindingDescription(), Material::PosNormalTexCoordVertex::getAttributeDescriptions());
    auto blinnPhongColorPipeline = std::make_unique<Engine::Ressources::Pipeline>(blinnPhongColorPipelineConfig);
    auto blinnPhongTexPipeline = std::make_unique<Engine::Ressources::Pipeline>(blinnPhongTexPipelineConfig);

    auto& ressourceManager = Engine::Ressources::RessourceManager::getInstance();


    auto blinnPhongColorTemplate = std::make_shared<Engine::Ressources::MaterialTemplate>(std::move(blinnPhongColorPipeline));
    auto blinnPhongTexTemplate = std::make_shared<Engine::Ressources::MaterialTemplate>(std::move(blinnPhongTexPipeline));
    ressourceManager.loadMaterialTemplate("blinnPhongColor", blinnPhongColorTemplate);
    ressourceManager.loadMaterialTemplate("blinnPhongTexture", blinnPhongTexTemplate);

    auto mat = std::make_shared<Engine::Ressources::Material>(blinnPhongColorTemplate, sizeof(Material::blinnPhongColor));
    ressourceManager.loadMaterial("mat", mat);
    auto defaultMat = Material::blinnPhongColor();
    defaultMat.diffuse = glm::vec3(2.0, 8.0, 1.0);
    defaultMat.shininess = 1.0;
    mat->updateData(&defaultMat);


    auto mat2 = std::make_shared<Engine::Ressources::Material>(blinnPhongColorTemplate, sizeof(Material::blinnPhongColor));
    ressourceManager.loadMaterial("mat 2", mat2);
    auto defaultMat2 = Material::blinnPhongColor();
    defaultMat2.diffuse = glm::vec3(1.0, 1.0, 1.0);
    defaultMat2.shininess = 1.0;
    mat2->updateData(&defaultMat2);


    auto woodTextureInfo = Engine::Ressources::Texture::TextureCreateInfo::getDefault("Assets/Game/wood.jpg");
    auto woodTexture = std::make_shared<Engine::Ressources::Texture>(woodTextureInfo);
    ressourceManager.loadTexture("wood", woodTexture);
    std::vector<VkDescriptorImageInfo> imageInfo = {woodTexture->createDescriptorImageInfo()};
    auto mat3 = std::make_shared<Engine::Ressources::Material>(blinnPhongTexTemplate, sizeof(Material::blinnPhongText), &imageInfo);
    ressourceManager.loadMaterial("mat 3", mat3);
    auto matStruct3 = Material::blinnPhongText();
    matStruct3.shininess = 1.0;
    mat3->updateData(&matStruct3);


    auto vertexBuffer = std::make_unique<Engine::Ressources::VertexBuffer>((void*)verticesTexCoord.data(), (size_t)verticesTexCoord.size() * sizeof(verticesTexCoord[0]));
    auto indexBuffer = std::make_unique<Engine::Ressources::IndexBuffer>((void*)indices.data(), (size_t)indices.size() * sizeof(indices[0]), indices.size());
    std::shared_ptr<Engine::Components::MeshRenderer> meshRenderer (new Engine::Components::MeshRenderer(entityWeak, mat3, std::move(vertexBuffer), std::move(indexBuffer)));

    std::shared_ptr<Engine::Components::Transform> transform (new Engine::Components::Transform(entityWeak));

    transform->position.y -= 1.0;
    transform->position.z -= 0.0;
    transform->setForwardVector(glm::vec3{0.3f, 0.2f, -1.5f});
    //transform->setForwardVector(glm::vec3{0.0f, 0.2f, -1.0f});

    std::shared_ptr<Components::BoxMovement> boxMOvement (new Components::BoxMovement(entityWeak));
    entity->addComponent(boxMOvement);
    entity->addComponent(transform);
    entity->addComponent(meshRenderer);

    
    
    std::shared_ptr<Engine::Entity> entity2 (new Engine::Entity("second entity"));
    std::weak_ptr<Engine::Entity> entityWeak2 (entity2);
    vertexBuffer = std::make_unique<Engine::Ressources::VertexBuffer>((void*)verticesColor.data(), (size_t)verticesColor.size() * sizeof(verticesColor[0]));
    indexBuffer = std::make_unique<Engine::Ressources::IndexBuffer>((void*)indices.data(), (size_t)indices.size() * sizeof(indices[0]), indices.size());
    std::shared_ptr<Engine::Components::MeshRenderer> meshRenderer2 (new Engine::Components::MeshRenderer(entityWeak2, mat, std::move(vertexBuffer), std::move(indexBuffer)));
    std::shared_ptr<Engine::Components::Transform> transform2 (new Engine::Components::Transform(entityWeak2));
    std::shared_ptr<Components::BoxMovement> boxMOvement2 (new Components::BoxMovement(entityWeak2));
    entity->addComponent(boxMOvement2);
    entity2->addComponent(meshRenderer2);
    entity2->addComponent(transform2);
    
    transform2->position.y += 2.0;
    transform2->position.z -= 2.0;
    
    std::shared_ptr<Engine::Entity> plane (new Engine::Entity("plane"));
    vertexBuffer = std::make_unique<Engine::Ressources::VertexBuffer>((void*)verticesColor.data(), (size_t)verticesColor.size() * sizeof(verticesColor[0]));
    indexBuffer = std::make_unique<Engine::Ressources::IndexBuffer>((void*)indices.data(), (size_t)indices.size() * sizeof(indices[0]), indices.size());
    std::shared_ptr<Engine::Components::MeshRenderer> meshRenderer3 (new Engine::Components::MeshRenderer(plane, mat2, std::move(vertexBuffer), std::move(indexBuffer)));
    std::shared_ptr<Engine::Components::Transform> transform3 (new Engine::Components::Transform(plane));
    plane->addComponent(meshRenderer3);
    plane->addComponent(transform3);
    
    transform3->position.y -= 3.0;
    
    transform3->scale.y = 0.01;
    transform3->scale.x = 10.0;
    transform3->scale.z = 10.0;


    std::shared_ptr<Engine::Entities::Camera> camera (new Engine::Entities::Camera("Camera"));
    camera->load(camera);
    std::weak_ptr<Engine::Entity> cameraWeak (camera);

    std::shared_ptr<Components::CameraMovement> component (new Components::CameraMovement(cameraWeak));
    camera->addComponent(component);
    
    std::shared_ptr<Engine::Entity> light (new Engine::Entity("Light"));
    std::weak_ptr<Engine::Entity> lightWeak (light);
    std::shared_ptr<Engine::Components::DirectionalLight> dirLight (new Engine::Components::DirectionalLight(lightWeak));
    dirLight->lightInfo.dir = glm::normalize(glm::vec3(0.3, 1.0, 1.0));
    dirLight->lightInfo.color = glm::vec3(1.0, 1.0, 1.0);
    light->addComponent(dirLight);
    
    std::shared_ptr<Engine::Entity> pointLight (new Engine::Entity("point Light"));
    std::weak_ptr<Engine::Entity> pointlightWeak (pointLight);
    std::shared_ptr<Engine::Components::PointLight> pointLightComp (new Engine::Components::PointLight(pointlightWeak));
    pointLightComp->lightInfo.pos = glm::normalize(glm::vec4(0.0, 1.0, 3.0, 1.0));
    pointLightComp->lightInfo.color = glm::vec4(1.0, 0.0, 0.2, 1.0);
    pointLightComp->lightInfo.constantAttenuation = 1.0f;
    pointLightComp->lightInfo.linearAttenuation = 0.09f;
    pointLightComp->lightInfo.quadraticAttenuation = 0.032f;
    pointLight->addComponent(pointLightComp);

    addEntity(entity2);
    addEntity(entity);
    addEntity(plane);
    addEntity(camera);
    addEntity(light);
    addEntity(pointLight);

};
}


