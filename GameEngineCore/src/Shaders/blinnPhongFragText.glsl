#version 450

const uint numPointLights = 100;
const uint numDirectionalLights = 10;

struct PointLight {
    vec4 pos;
    vec4 color;

    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

struct DirectionalLight {
    vec4 dir;
    vec4 color;
};

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(set = 0, binding = 1) uniform lightBufferObject {
    uint nbPointLights;
    uint nbDirectionalLights;
    PointLight pointLights[numPointLights];
    DirectionalLight directionalLights[numDirectionalLights];
} lights;

layout(set = 2, binding = 0) uniform Material {
    float shininess;
} material;

layout(set = 2, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec4 inVertPos;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 result = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 diffuse = texture(texSampler, inTexCoord);
    for (uint i = 0; i < lights.nbPointLights; i++) {
        PointLight light = lights.pointLights[i];
        vec4 lightViewPos = ubo.view * light.pos;
        vec4 lightDir = normalize(lightViewPos - inVertPos);
        float distance = distance(lightViewPos, inVertPos);
        float lambertian = max(dot(lightDir, inNormal), 0.0);
        float specular = 0.0;

        if (lambertian > 0) {
            vec4 viewDir = normalize(-inVertPos);
            vec4 halfDir = normalize(lightDir + viewDir);
            float specAngle = max(dot(halfDir, inNormal), 0.0);
            specular = pow(specAngle, material.shininess);
        }

        float attenuation = 1.0 / (light.constantAttenuation + light.linearAttenuation * distance +
                    light.quadraticAttenuation * (distance * distance));

        result += (lambertian * light.color * diffuse * specular) * attenuation;
    }
    for (uint i = 0; i < lights.nbDirectionalLights; i++) {
        DirectionalLight light = lights.directionalLights[i];
        float lambertian = max(dot(light.dir, inNormal), 0.0);
        float specular = 0.0;

        if (lambertian > 0) {
            vec4 viewDir = normalize(-inVertPos);
            vec4 halfDir = normalize(light.dir + viewDir);
            float specAngle = max(dot(halfDir, inNormal), 0.0);
            specular = pow(specAngle, material.shininess);
        }

        result += lambertian * light.color * diffuse * specular;
    }
    outColor = result;
}
