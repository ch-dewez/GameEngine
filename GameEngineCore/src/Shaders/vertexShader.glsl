#version 450

layout (set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout (set = 1, binding = 0) uniform ModelUniformBufferObject {
    mat4 model;
} modelUbo;  // Declare as an array to use with dynamic offsets

layout(set=2, binding=0) uniform Material {
    vec3 color;
} material;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    vec4 worldPos = modelUbo.model * vec4(inPosition, 1.0);  // Use index 0 since dynamic offset will handle the actual index
    vec4 viewPos = ubo.view * worldPos;
    gl_Position = ubo.proj * viewPos;
    fragColor = material.color;
}
