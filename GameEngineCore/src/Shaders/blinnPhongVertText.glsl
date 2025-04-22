#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(set = 1, binding = 0) uniform ModelUniformBufferObject {
    mat4 model;
} modelUbo; // Declare as an array to use with dynamic offsets

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 outVertPos;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec2 outTexCoord;

void main() {
    vec4 worldPos = modelUbo.model * vec4(inPosition, 1.0); // Use index 0 since dynamic offset will handle the actual index
    vec4 viewPos = ubo.view * worldPos;
    gl_Position = ubo.proj * viewPos;
    outVertPos = viewPos;
    outNormal = vec4(inNormal, 1.0);
    outTexCoord = inTexCoord;
}
