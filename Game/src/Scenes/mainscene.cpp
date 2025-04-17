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
#include "../Materials/defaultMaterial.h"
#include <memory>
#include <iostream>

namespace Game {

const std::vector<Vertex> vertices = {
    // Front face
    {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},  // 0
    {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},   // 1
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},    // 2
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},   // 3

    // Back face
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // 4
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},  // 5
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},   // 6
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}   // 7
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

const std::vector<uint16_t> indices = {
    // Front face (CCW from front)
    0, 1, 2, 2, 3, 0,
    // Back face (CCW from back)
    5, 4, 7, 7, 6, 5,
    // Left face (CCW from left)
    4, 0, 3, 3, 7, 4,
    // Right face (CCW from right)
    1, 5, 6, 6, 2, 1,
    // Top face (CCW from top)
    3, 2, 6, 6, 7, 3,
    // Bottom face (CCW from bottom)
    4, 5, 1, 1, 0, 4
};

void MainScene::initialize(){
    //std::shared_ptr<Engine::Entities::Camera> camera();

    std::shared_ptr<Engine::Entity> entity (new Engine::Entity("First entity"));
    std::weak_ptr<Engine::Entity> entityWeak (entity);


    auto config = Engine::Ressources::PipelineConfigInfo::defaultPipelineConfigInfo("shaders/GameEngineCore/vertexShader.glsl.spv", "shaders/GameEngineCore/fragmentShader.glsl.spv");
    //auto config = Engine::Ressources::PipelineConfigInfo::defaultPipelineConfigInfo("build/shaders/GameEngineCore/vertexShader.glsl.spv", "build/shaders/GameEngineCore/fragmentShader.glsl.spv");
    auto pipeline = std::make_unique<Engine::Ressources::Pipeline>(config);
    auto vertexBuffer = std::make_unique<Engine::Ressources::VertexBuffer>((void*)vertices.data(), (size_t)vertices.size() * sizeof(vertices[0]));
    auto indexBuffer = std::make_unique<Engine::Ressources::IndexBuffer>((void*)indices.data(), (size_t)indices.size() * sizeof(indices[0]), indices.size());

    auto& ressourceManager = Engine::Ressources::RessourceManager::getInstance();

    auto matTemplate = ressourceManager.createMaterialTemplate("default Mat template", sizeof(Material::defaultMaterial), std::move(pipeline));

    auto mat = ressourceManager.createMaterial("default mat", matTemplate);
    auto defaultMat = Material::defaultMaterial();
    defaultMat.color = glm::vec3(1.0, 1.0, 0.0);
    mat->updateData(&defaultMat);


    auto mat2 = ressourceManager.createMaterial("default mat2", matTemplate);
    auto defaultMat2 = Material::defaultMaterial();
    defaultMat2.color = glm::vec3(1.0, 1.0, 1.0);
    mat2->updateData(&defaultMat2);


    std::shared_ptr<Engine::Components::MeshRenderer> meshRenderer (new Engine::Components::MeshRenderer(entityWeak, mat, std::move(vertexBuffer), std::move(indexBuffer)));

    std::shared_ptr<Engine::Components::Transform> transform (new Engine::Components::Transform(entityWeak));

    transform->position.y += 0.0;
    transform->position.z -= 0.0;
    //transform->setForwardVector(glm::vec3{0.3f, 0.2f, -1.5f});
    //transform->setForwardVector(glm::vec3{0.0f, 0.2f, -1.0f});

    std::shared_ptr<Components::BoxMovement> boxMOvement (new Components::BoxMovement(entityWeak));
    entity->addComponent(boxMOvement);
    entity->addComponent(transform);
    entity->addComponent(meshRenderer);

    
    
    std::shared_ptr<Engine::Entity> entity2 (new Engine::Entity("second entity"));
    std::weak_ptr<Engine::Entity> entityWeak2 (entity2);
    vertexBuffer = std::make_unique<Engine::Ressources::VertexBuffer>((void*)vertices.data(), (size_t)vertices.size() * sizeof(vertices[0]));
    indexBuffer = std::make_unique<Engine::Ressources::IndexBuffer>((void*)indices.data(), (size_t)indices.size() * sizeof(indices[0]), indices.size());
    std::shared_ptr<Engine::Components::MeshRenderer> meshRenderer2 (new Engine::Components::MeshRenderer(entityWeak2, mat2, std::move(vertexBuffer), std::move(indexBuffer)));
    std::shared_ptr<Engine::Components::Transform> transform2 (new Engine::Components::Transform(entityWeak2));
    std::shared_ptr<Components::BoxMovement> boxMOvement2 (new Components::BoxMovement(entityWeak2));
    entity->addComponent(boxMOvement2);
    entity2->addComponent(meshRenderer2);
    entity2->addComponent(transform2);

    transform2->position.y += 2.0;
    transform2->position.z -= 2.0;

    std::shared_ptr<Engine::Entities::Camera> camera (new Engine::Entities::Camera("Camera"));
    camera->load(camera);
    std::weak_ptr<Engine::Entity> cameraWeak (camera);

    std::shared_ptr<Components::CameraMovement> component (new Components::CameraMovement(cameraWeak));
    //camera->addComponent(component);
    

    addEntity(entity2);
    addEntity(entity);
    addEntity(camera);

};
}


