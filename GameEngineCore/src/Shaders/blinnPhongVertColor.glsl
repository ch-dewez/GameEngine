#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

// const uint numPointLights = 100;
// const uint numDirectionalLights = 10;
//
// struct PointLight {
//     vec3 pos;
//     vec3 color;
//
//     float constantAttenuation;
//     float linearAttenuation;
//     float quadraticAttenuation;
// };
//
// struct DirectionalLight {
//     vec3 dir;
//     vec3 color;
// };
//
// layout(set = 0, binding = 1) uniform lightBufferObject {
//     uint nbPointLights;
//     uint nbDirectionalLights;
//     PointLight pointLights[numPointLights];
//     DirectionalLight directionalLights[numDirectionalLights];
// } lights;

layout(set = 1, binding = 0) uniform ModelUniformBufferObject {
    mat4 model;
} modelUbo; // Declare as an array to use with dynamic offsets

// layout(set = 2, binding = 0) uniform Material {
//     vec3 diffuse;
//     float shininess;
// } material;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec4 outVertPos;
layout(location = 1) out vec4 outNormal;

void main() {
    vec4 worldPos = modelUbo.model * vec4(inPosition, 1.0); // Use index 0 since dynamic offset will handle the actual index
    vec4 viewPos = ubo.view * worldPos;
    gl_Position = ubo.proj * viewPos;
    outVertPos = viewPos;
    mat3 normalMatrix = mat3(transpose(inverse(modelUbo.model)));
    vec3 rotatedNormal = normalize(normalMatrix * inNormal);
    outNormal = vec4(rotatedNormal, 0.0);
}
