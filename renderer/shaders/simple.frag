#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform MaterialFactor {
    vec4 baseColorFactor;
    vec4 metallicRoughnessFactor; 
} material;

layout(set = 0, binding = 1) uniform sampler2D baseColorTexture;
layout(set = 0, binding = 2) uniform sampler2D metallicRoughnessTexture;


void main() {
    outColor = material.baseColorFactor * texture(baseColorTexture, fragTexCoord);
   // outColor = vec4(1.0, 1.0, 1.0, 1.0);
}