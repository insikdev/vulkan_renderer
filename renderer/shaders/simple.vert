#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(std140, set = 0, binding = 0) uniform ModelUniformData {
    mat4 world;
} modelUBO;

layout(std140, set = 0, binding = 1) uniform GlobalUniformData {
    mat4 view;
    mat4 proj;
} globalUBO;

void main() {
    gl_Position = globalUBO.proj * globalUBO.view * modelUBO.world * vec4(inPos, 0.0, 1.0);
    fragColor = inColor;
}