#pragma once
#include "Core/Renderer/Renderer.h"
#include "IndexBuffer.h"
#include "vertexBuffer.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace Engine {
namespace Ressources {

class Mesh {
public:
    enum class VertexDataType {
        positions,
        normals,
        tex_coords
    };
public:
    Mesh();
    ~Mesh();

    Mesh(const std::string& path);

    Mesh(std::vector<uint32_t> indices, void* vertexData, size_t size);

    void loadObj(const std::string& path, uint8_t infoToLoad = 0xFF);

    const char* vertexDataTypeToCharPointer(VertexDataType dataType);
    
    // use string if not in vertex data type (it's just not to do typos)
    void setOrCreateChannel(VertexDataType dataType, void* data, size_t sizeOfOneElement, size_t nbOfElement);
    void setOrCreateChannel(const char* identifier, void* data, size_t sizeOfOneElement, size_t nbOfElement);
    void setOrCreateChannelAndCopyData(VertexDataType dataType, void* data, size_t sizeOfElement, size_t nbOfElement);
    void setOrCreateChannelAndCopyData(const char* identifier, void* data, size_t sizeOfElement, size_t nbOfElement);


    void removeChannel(VertexDataType dataType);
    void removeChannel(const char* identifier);

    void setIndices(std::vector<uint32_t> indices) {m_indices = std::move(indices);}; // don't want to "guess" the behavior so std::move

    template<typename T>
    T* getChannel(const char* identifier);

    // if data already in gpu update it
    void uploadDataToGpu();

    // no way of getting it back
    void removeDataFromCpu();

    // if data is on gpu
    void bind(Engine::Renderer::Renderer::FrameInfo frameInfo);
    void draw(Engine::Renderer::Renderer::FrameInfo frameInfo);
private:


    // The mesh data can be stored on the cpu or on the gpu. I obviously don't want to get the data from the gpu so at first the data is on the cpu and you can upload it to gpu to render it. You sometimes want the data to be on the cpu to calculate thing like for rigidbody
    enum class State {
        storedNoWhere,
        storedOnCpu,
        storedOnGpu,
        storedOnBoth
    };

    State m_state = Mesh::State::storedNoWhere;
    
    // I need to store each channel like this because some system need to access the position so we can do getChannel<glm::vec3>("position") but if we don't do that you can't know if position if the first thing or the second
    // but this make the uploading to the gpu more difficult
    struct Channel {
        size_t sizeOfElement;
        size_t nbOfElement;
        void* data;
    };

    std::unordered_map<std::string,Channel> m_channels;
    std::vector<std::string>              m_channelOrder;

    std::vector<uint32_t> m_indices;

    IndexBuffer* m_indexBuffer = nullptr;
    VertexBuffer* m_vertexBuffer = nullptr;
};

}
}
