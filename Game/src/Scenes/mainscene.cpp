#include "mainscene.h"
#include "../Components/CameraMovement.h"
#include "../Components/BoxMovement.h"
#include "../Materials/defaultMaterial.h"
#include <memory>
#include <vector>

namespace Game {

// Positions for cube vertices (24 vertices, duplicated as needed)
const std::vector<glm::vec3> positions = {
    // Front face
    {-0.5f, -0.5f,  0.5f},  // 0
    { 0.5f, -0.5f,  0.5f},  // 1
    { 0.5f,  0.5f,  0.5f},  // 2
    {-0.5f,  0.5f,  0.5f},  // 3
    // Back face
    {-0.5f, -0.5f, -0.5f},  // 4
    { 0.5f, -0.5f, -0.5f},  // 5
    { 0.5f,  0.5f, -0.5f},  // 6
    {-0.5f,  0.5f, -0.5f},  // 7
    // Left face
    {-0.5f, -0.5f, -0.5f},  // 8
    {-0.5f, -0.5f,  0.5f},  // 9
    {-0.5f,  0.5f,  0.5f},  // 10
    {-0.5f,  0.5f, -0.5f},  // 11
    // Right face
    { 0.5f, -0.5f,  0.5f},  // 12
    { 0.5f, -0.5f, -0.5f},  // 13
    { 0.5f,  0.5f, -0.5f},  // 14
    { 0.5f,  0.5f,  0.5f},  // 15
    // Top face
    {-0.5f,  0.5f,  0.5f},  // 16
    { 0.5f,  0.5f,  0.5f},  // 17
    { 0.5f,  0.5f, -0.5f},  // 18
    {-0.5f,  0.5f, -0.5f},  // 19
    // Bottom face
    {-0.5f, -0.5f, -0.5f},  // 20
    { 0.5f, -0.5f, -0.5f},  // 21
    { 0.5f, -0.5f,  0.5f},  // 22
    {-0.5f, -0.5f,  0.5f}   // 23
};

// Normals for each vertex (24 vertices)
const std::vector<glm::vec3> normals = {
    // Front face (normal: 0, 0, 1)
    {0.0f, 0.0f, 1.0f},  // 0
    {0.0f, 0.0f, 1.0f},  // 1
    {0.0f, 0.0f, 1.0f},  // 2
    {0.0f, 0.0f, 1.0f},  // 3
    // Back face (normal: 0, 0, -1)
    {0.0f, 0.0f, -1.0f},  // 4
    {0.0f, 0.0f, -1.0f},  // 5
    {0.0f, 0.0f, -1.0f},  // 6
    {0.0f, 0.0f, -1.0f},  // 7
    // Left face (normal: -1, 0, 0)
    {-1.0f, 0.0f, 0.0f},  // 8
    {-1.0f, 0.0f, 0.0f},  // 9
    {-1.0f, 0.0f, 0.0f},  // 10
    {-1.0f, 0.0f, 0.0f},  // 11
    // Right face (normal: 1, 0, 0)
    {1.0f, 0.0f, 0.0f},  // 12
    {1.0f, 0.0f, 0.0f},  // 13
    {1.0f, 0.0f, 0.0f},  // 14
    {1.0f, 0.0f, 0.0f},  // 15
    // Top face (normal: 0, 1, 0)
    {0.0f, 1.0f, 0.0f},  // 16
    {0.0f, 1.0f, 0.0f},  // 17
    {0.0f, 1.0f, 0.0f},  // 18
    {0.0f, 1.0f, 0.0f},  // 19
    // Bottom face (normal: 0, -1, 0)
    {0.0f, -1.0f, 0.0f},  // 20
    {0.0f, -1.0f, 0.0f},  // 21
    {0.0f, -1.0f, 0.0f},  // 22
    {0.0f, -1.0f, 0.0f}   // 23
};

// Texture coordinates for each vertex (24 vertices)
const std::vector<glm::vec2> texCoords = {
    // Front face
    {0.0f, 0.0f},  // 0
    {1.0f, 0.0f},  // 1
    {1.0f, 1.0f},  // 2
    {0.0f, 1.0f},  // 3
    // Back face
    {1.0f, 0.0f},  // 4
    {0.0f, 0.0f},  // 5
    {0.0f, 1.0f},  // 6
    {1.0f, 1.0f},  // 7
    // Left face
    {0.0f, 0.0f},  // 8
    {1.0f, 0.0f},  // 9
    {1.0f, 1.0f},  // 10
    {0.0f, 1.0f},  // 11
    // Right face
    {0.0f, 0.0f},  // 12
    {1.0f, 0.0f},  // 13
    {1.0f, 1.0f},  // 14
    {0.0f, 1.0f},  // 15
    // Top face
    {0.0f, 0.0f},  // 16
    {1.0f, 0.0f},  // 17
    {1.0f, 1.0f},  // 18
    {0.0f, 1.0f},  // 19
    // Bottom face
    {0.0f, 0.0f},  // 20
    {1.0f, 0.0f},  // 21
    {1.0f, 1.0f},  // 22
    {0.0f, 1.0f}   // 23
};


const std::vector<uint32_t> indices = {
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


void MainScene::initObject(){
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
    defaultMat.diffuse = glm::vec3(1.0, .7, 0.3);
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

    /*auto cubeTexMesh = std::make_shared<Engine::Ressources::Mesh>(indices, (void*)verticesTexCoord.data(), (size_t)verticesTexCoord.size() * sizeof(verticesTexCoord[0]));*/

    auto cubeTexMesh = std::make_shared<Engine::Ressources::Mesh>();
    cubeTexMesh->setOrCreateChannelAndCopyData("positions", (void*)positions.data(), sizeof(positions[0]), positions.size());
    cubeTexMesh->setOrCreateChannelAndCopyData("normals", (void*)normals.data(), sizeof(normals[0]), normals.size());
    cubeTexMesh->setOrCreateChannelAndCopyData("texture coord", (void*)texCoords.data(), sizeof(texCoords[0]), texCoords.size());
    cubeTexMesh->setIndices(std::vector(indices));
    cubeTexMesh->uploadDataToGpu();
    cubeTexMesh->removeDataFromCpu();

    /*//auto cubeMesh = std::make_shared<Engine::Ressources::Mesh>(indices, (void*)verticesColor.data(), (size_t)verticesColor.size() * sizeof(verticesColor[0]));*/
    auto cubeMesh = std::make_shared<Engine::Ressources::Mesh>();
    cubeMesh->setOrCreateChannelAndCopyData("positions", (void*)positions.data(), sizeof(positions[0]), positions.size());
    cubeMesh->setOrCreateChannelAndCopyData("normals", (void*)normals.data(), sizeof(normals[0]), normals.size());
    cubeMesh->setIndices(indices);
    cubeMesh->uploadDataToGpu();
    cubeMesh->removeDataFromCpu();
    ressourceManager.loadMesh("cubeTexMesh", cubeTexMesh);
    ressourceManager.loadMesh("cubeMesh", cubeMesh);

    auto model = std::make_shared<Engine::Ressources::Mesh>();
    {
        model->loadObj("Assets/Game/model.obj");
        LogDebug("finish loading model");
        model->uploadDataToGpu();
        model->removeDataFromCpu();
        ressourceManager.loadMesh("model", model);
    }

    auto modelTextureInfo = Engine::Ressources::Texture::TextureCreateInfo::getDefault("Assets/Game/modelTexture.png");
    auto modelTexture = std::make_shared<Engine::Ressources::Texture>(modelTextureInfo);
    ressourceManager.loadTexture("model Texture", modelTexture);
    std::vector<VkDescriptorImageInfo> modelImageiNfo = {modelTexture->createDescriptorImageInfo()};
    auto modelMat = std::make_shared<Engine::Ressources::Material>(blinnPhongTexTemplate, sizeof(Material::blinnPhongText), &modelImageiNfo);
    ressourceManager.loadMaterial("model mat", modelMat);
    auto modelMatStruct = Material::blinnPhongText();
    modelMatStruct.shininess = 1.0;
    modelMat->updateData(&matStruct3);


    {
        auto& entity = addEntity("model");
        auto& transform = entity.addComponent<Engine::Components::Transform>();
        auto& renderer = entity.addComponent<Engine::Components::MeshRenderer>(modelMat, model);
        transform.rotation = {-0.0f, 0.0f, 0.7071068f, 0.7071068f};
        transform.scale = {10.0f, 10.f, 10.f};
    }

    {
        auto& entity = addEntity("First entity");
        auto& renderer = entity.addComponent<Engine::Components::MeshRenderer>(mat3, cubeTexMesh);
        auto& transform = entity.addComponent<Engine::Components::Transform>();

        transform.position.y += 5.5;
        glm::vec3 ibodyinv = Engine::Components::RigidBody::InvInertiaCuboidDensity(transform.scale.x, transform.scale.y, transform.scale.z);
        auto& rb = entity.addComponent<Engine::Components::RigidBody>(glm::vec3(0.0f), ibodyinv);
        //rb->setGravity(glm::vec3(0.0f));
        //rb->m_angularMomentum = {1.0, 1.0, 0.0};
        //rb->addForce(glm::vec3(-0.76f, 0.0f, 0.0f), Engine::Components::ForceMode::Impulse);
        entity.addComponent<Engine::Components::CubeCollider>();
    }
    /**/
    /*{*/
    /*    auto& entity = addEntity("second entity");*/
    /*    auto& transform = entity.addComponent<Engine::Components::Transform>();*/
    /*    transform.position.y +=2.5;*/
    /*    entity.addComponent<Engine::Components::MeshRenderer>(mat, cubeMesh);*/
    /**/
    /*    glm::vec3 ibodyinv = Engine::Components::RigidBody::InvInertiaCuboidDensity(transform.scale.x, transform.scale.y, transform.scale.z);*/
    /*    auto& rb = entity.addComponent<Engine::Components::RigidBody>(glm::vec3(0.0f), ibodyinv);*/
    /**/
    /*    entity.addComponent<Engine::Components::CubeCollider>();*/
    /*}*/

    {
        auto& entity = addEntity("Camera");
        entity.addComponent<Engine::Components::Transform>();
        entity.addComponent<Engine::Components::Camera>();
        entity.addComponent<Components::CameraMovement>();
        entity.tags.push_back("Main Camera");
    }

    {
        auto& entity = addEntity("plane");
        auto& transform = entity.addComponent<Engine::Components::Transform>();
        transform.position.y -=3.0;
        transform.scale = {100.0, 0.1, 100.0};
        entity.addComponent<Engine::Components::MeshRenderer>(mat, cubeMesh);
        entity.addComponent<Engine::Components::CubeCollider>();
    }

    //TODO: make prefab work

    /*{*/
    /*    Engine::Entities::Camera& camera = addEntity<Engine::Entities::Camera>("Camera");*/
    /*    camera.load();*/
    /**/
    /*    camera.addComponent<Components::CameraMovement>();*/
    /**/
    /*    Engine::Components::Transform& transform = *camera.getComponent<Engine::Components::Transform>().value();*/
    /*    transform.scale = {0.1, 0.1, 0.1};*/
    /*}*/
    
    {
        auto& entity = addEntity("dir ligh");
        auto& dirLight = entity.addComponent<Engine::Components::DirectionalLight>();
        dirLight.lightInfo.dir = glm::normalize(glm::vec3(-0.3, 1.0, 1.0));
        dirLight.lightInfo.color = glm::vec3(1.0, 1.0, 1.0);
    }
    
    {
        auto& entity = addEntity("point ligh");
        auto& pointLight = entity.addComponent<Engine::Components::PointLight>();
        pointLight.lightInfo.pos = glm::normalize(glm::vec4(0.0, 1.0, 0.0, 1.0));
        pointLight.lightInfo.color = glm::vec4(1.0, 0.0, 0.2, 1.0);
        pointLight.lightInfo.constantAttenuation = 1.0f;
        pointLight.lightInfo.linearAttenuation = 0.09f;
        pointLight.lightInfo.quadraticAttenuation = 0.032f;
    }

    {
        auto& entity = addEntity("point ligh");
        auto& pointLight = entity.addComponent<Engine::Components::PointLight>();
        pointLight.lightInfo.pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        pointLight.lightInfo.color = glm::vec4(0.0, 1.0, 0.2, 1.0);
        pointLight.lightInfo.constantAttenuation = 1.0f;
        pointLight.lightInfo.linearAttenuation = 0.09f;
        pointLight.lightInfo.quadraticAttenuation = 0.032f;
    }
};
}


