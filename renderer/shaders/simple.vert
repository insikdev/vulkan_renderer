#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(std140, set = 0, binding = 0) uniform ModelUniformData {
    mat4 world;
} modelUBO;

layout(std140, set = 0, binding = 1) uniform GlobalUniformData {
    mat4 view;
    mat4 proj;
} globalUBO;

layout(set = 0, binding = 2) uniform sampler2D texSampler;


void main() {
    gl_Position = globalUBO.proj * globalUBO.view * modelUBO.world * vec4(inPos, 0.0, 1.0);
    fragTexCoord = inTexCoord;
    fragColor = inColor;
}