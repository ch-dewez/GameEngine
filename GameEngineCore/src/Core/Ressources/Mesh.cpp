#include "Mesh.h"
#include "Core/Log/Log.h"
#include <_string.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "tiny_obj_loader.h"

namespace Engine {
namespace Ressources {

Mesh::Mesh() {

};

Mesh::Mesh(const std::string& path){
    loadObj(path);
}

Mesh::Mesh(std::vector<uint32_t> indices, void* vertexData, size_t size)
{
    m_vertexBuffer = new Engine::Ressources::VertexBuffer(vertexData, size);
    m_indexBuffer = new Engine::Ressources::IndexBuffer(indices.data(), (size_t)indices.size() * sizeof(indices[0]), indices.size());
    m_state = State::storedOnGpu;
}

Mesh::~Mesh() {
    LogDebug("Mehs deconstructor");
    LogDebug(m_channelOrder.size());
    while (m_channelOrder.size() > 0){
        LogDebug(m_channelOrder.size());
        removeChannel(m_channelOrder[0].c_str());
        LogDebug(m_channelOrder.size());
    }

    delete m_indexBuffer;
    delete m_vertexBuffer;
}

// TODO: vertex deduplication
void Mesh::loadObj(const std::string& path, uint8_t infoToLoad){
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
    Assert(success, "Failed to load obj, warning : ", warn, ", error : ", err);
    LogWarning(warn);

    uint32_t nbPositions = 0;
    uint32_t nbNormals = 0;
    uint32_t nbTex_coords = 0;
    uint32_t nbIndices = 0;
    
    for (const auto& shape : shapes){
        for (const auto& index : shape.mesh.indices){
            if (infoToLoad & (1 << (int)VertexDataType::positions)) {
                nbPositions ++;
            }

            if (infoToLoad & (1 << (int)VertexDataType::normals)) {
                nbNormals ++;
            }

            if (infoToLoad & (1 << (int)VertexDataType::tex_coords)) {
                nbTex_coords ++;
            }

            nbIndices++;
        }
    }

    glm::vec3* positions = (glm::vec3*)malloc(sizeof(glm::vec3) * nbPositions);
    glm::vec3* normals = (glm::vec3*)malloc(sizeof(glm::vec3) * nbNormals);
    glm::vec2* tex_coords = (glm::vec2*)malloc(sizeof(glm::vec2) * nbTex_coords);
    std::vector<uint32_t> indices(nbIndices);

    uint32_t arrIdx = 0;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            if (infoToLoad & (1 << (int)VertexDataType::positions)) {
                positions[arrIdx] = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };
            }

            if (infoToLoad & (1 << (int)VertexDataType::normals)) {
                normals[arrIdx] = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }

            if (infoToLoad & (1 << (int)VertexDataType::tex_coords)) {
                tex_coords[arrIdx] = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1- attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }


            indices[arrIdx] = arrIdx;
            arrIdx ++;
        }
    }

    if (infoToLoad & (1 << (int)VertexDataType::positions))
        setOrCreateChannel(VertexDataType::positions, positions, sizeof(positions[0]), sizeof(positions[0]) * nbPositions);
    if (infoToLoad & (1 << (int)VertexDataType::normals))
        setOrCreateChannel(VertexDataType::normals, normals, sizeof(normals[0]), sizeof(normals[0]) * nbNormals);
    if (infoToLoad & (1 << (int)VertexDataType::tex_coords))
        setOrCreateChannel(VertexDataType::tex_coords, tex_coords, sizeof(tex_coords[0]), sizeof(tex_coords[0]) * nbTex_coords);

    setIndices(std::move(indices));
}

const char* Mesh::vertexDataTypeToCharPointer(VertexDataType dataType){
    switch (dataType) {
        case VertexDataType::positions:
            return "positions";
            break;
        case VertexDataType::normals:
            return "normals";
            break;
        case VertexDataType::tex_coords:
            return "tex_coords";
            break;
    }

}


void Mesh::removeChannel(VertexDataType dataType){
    removeChannel(vertexDataTypeToCharPointer(dataType));
}

void Mesh::removeChannel(const char* identifier){
    LogDebug("freeing channel");
    free(m_channels[identifier].data);
    m_channels.erase(identifier);
    int index = -1;
    for (int i=0;i<m_channelOrder.size();i++){
        if (m_channelOrder[i] == identifier){
            index = i;
            break;
        }
    }
    m_channelOrder.erase(m_channelOrder.begin() + index);
}

void Mesh::setOrCreateChannel(VertexDataType dataType, void* data, size_t sizeOfOneElement, size_t nbOfElement){
    return setOrCreateChannel(vertexDataTypeToCharPointer(dataType), data, sizeOfOneElement, nbOfElement);
}

void Mesh::setOrCreateChannelAndCopyData(VertexDataType dataType, void* data, size_t sizeOfOneElement, size_t nbOfElement){
    return setOrCreateChannelAndCopyData(vertexDataTypeToCharPointer(dataType), data, sizeOfOneElement, nbOfElement);
}

void Mesh::setOrCreateChannelAndCopyData(const char* identifier, void* data, size_t sizeOfElement, size_t nbOfElement){
    void* pt = malloc(sizeOfElement * nbOfElement);
    memcpy(pt, data, sizeOfElement * nbOfElement);
    setOrCreateChannel(identifier, pt, sizeOfElement,nbOfElement);
}

void Mesh::setOrCreateChannel(const char* identifier, void* data, size_t sizeOfElement, size_t nbOfElement){
    if (m_channels.contains(identifier)){
        LogDebug("removing channel because already there");
        removeChannel(identifier);
    }

    Channel channel;
    channel.data = data;
    channel.sizeOfElement = sizeOfElement;
    channel.nbOfElement = nbOfElement;
    m_channels[identifier] = channel;
    m_channelOrder.push_back(identifier);
};

template<typename T>
T* Mesh::getChannel(const char* identifier) {
    Assert(m_channels.contains(identifier), "Trying to get channel but does not exist");
    return m_channels[identifier];
};

void Mesh::uploadDataToGpu() {
    Assert((m_state != State::storedOnGpu), "try to upload data to gpu but no data on cpu");

    // calculate the size of the mesh.
    int nbVertices = m_channels.begin()->second.nbOfElement;
    Assert(nbVertices > 0, "vertices not > 0 can't upload mesh data to gpu if no data");
    size_t stride = 0;
    for (auto channel : m_channels){
        stride += channel.second.sizeOfElement;
    }
    size_t size = stride * nbVertices;

    void* data = malloc(size);
    void* writePointer = data;
    for (int element=0;element<m_channels.begin()->second.nbOfElement;element++){
        for (auto channelName : m_channelOrder){
            auto& channel = m_channels[channelName];
           memcpy(writePointer, (void*)((char*)channel.data + element * channel.sizeOfElement), channel.sizeOfElement);
            writePointer = (char*)writePointer + channel.sizeOfElement;
        }
    }

    if (m_state != State::storedOnCpu && m_state != State::storedNoWhere) {
        m_vertexBuffer->updateData(data, size);
        m_indexBuffer->updateData(m_indices.data(), m_indices.size() * sizeof(m_indices[0]));
    }else {
        // don't think this can happen
        if (m_indexBuffer){
            delete m_indexBuffer;
        }
        if (m_vertexBuffer){
            delete m_vertexBuffer;
        }

        m_vertexBuffer = new Engine::Ressources::VertexBuffer(data, size);
        m_indexBuffer = new Engine::Ressources::IndexBuffer(m_indices.data(), (size_t)m_indices.size() * sizeof(m_indices[0]), m_indices.size());
    }

    m_state = State::storedOnBoth;

    free(data);
}


void Mesh::removeDataFromCpu() {
    AssertWarn((m_state != State::storedOnGpu && m_state != State::storedNoWhere), "try to remove data from cpu but already not there");

    if (m_state == State::storedOnBoth) {
        m_state = State::storedOnGpu;
    } else if (m_state == State::storedOnCpu) {
        m_state = State::storedNoWhere;
    }

    while (m_channelOrder.size() > 0){
        removeChannel(m_channelOrder[0].c_str());
    }
};

void Mesh::bind(Engine::Renderer::Renderer::FrameInfo frameInfo){
    Assert((m_state != State::storedOnCpu), "Try to draw mesh but is stored on the cpu. You need to call uploadDataToGpu()");
    auto& api = ::Engine::Renderer::VulkanApi::Instance();

    VkBuffer vertexBuffers[] = {m_vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    api.cmdBindVertexBuffers(frameInfo.commandBuffer, 0, 1, vertexBuffers, offsets);
    api.cmdBindIndexBuffer(frameInfo.commandBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);

};

void Mesh::draw(Engine::Renderer::Renderer::FrameInfo frameInfo){
    if (m_state == State::storedOnCpu){
        LogError("Try to draw mesh but is stored on the cpu. You need to call uploadDataToGpu()");
    }
    auto& api = ::Engine::Renderer::VulkanApi::Instance();
    api.cmdDrawIndexed(frameInfo.commandBuffer, m_indexBuffer->indexCount, 1, 0, 0, 0);
};

}
}
