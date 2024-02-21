#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(set = 1, binding = 0) uniform Node {
    mat4 world;
} nodeUBO;

layout(set = 1, binding = 1) uniform Global {
    mat4 view;
    mat4 proj;
} globalUBO;


void main() {
    gl_Position = globalUBO.proj * globalUBO.view * nodeUBO.world * vec4(inPos, 1.0);
    
    fragTexCoord = inTexCoord;
    fragColor = inColor;
}