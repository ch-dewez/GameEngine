#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace Engine {
namespace Renderer {

static constexpr uint32_t MAX_POINT_LIGHTS = 100; // make numbers the same as in shader
static constexpr uint32_t MAX_DIRECTIONAL_LIGHTS = 10;


struct PointLight {
    alignas(16) glm::vec4 pos;
    alignas(16) glm::vec4 color;
    /*glm::vec3 pos;*/
    /*glm::vec3 color;*/

    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

struct DirectionalLight {
    alignas(16) glm::vec3 dir;
    alignas(16) glm::vec3 color;
    /*glm::vec3 dir;*/
    /*glm::vec3 color;*/
};


struct LightEnvironment {
    uint32_t nbPointLight;
    uint32_t nbDirectionalLight;
    PointLight pointLights[MAX_POINT_LIGHTS];
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

}
}
