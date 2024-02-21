#pragma once

#include <tiny_gltf.h>
// #include "mesh.h"

namespace glTF {
struct Material {
    glm::vec4 baseColorFactor { 1.0f };
    glm::vec4 metallicRoughnessFactor { 1.0f }; // [0] : metallic, [1] : roughness, [2] : dummy, [3] : dummy
    VK::Image baseColorTexture;
    VK::Image metallicRoughnessTexture;

    VK::DescriptorSet descriptorSet;
};

struct Primitive {
    VK::Buffer vertexBuffer;
    VK::Buffer indexBuffer;
    // VK::Buffer uniformBuffer;
    // VK::DescriptorSet descriptorSet;
    // MeshUniformData uniformData {};
    uint32_t indexCount {};
};

struct Mesh {
    std::vector<Primitive> primitives;
};

struct Node {
    std::optional<int> cameraIndex { std::nullopt };
    std::optional<int> skinIndex { std::nullopt };
    glm::mat4 matrix { 1.0f };
    std::optional<int> meshIndex { std::nullopt };
    glm::quat rotation { 1.0f, 0.0f, 0.0f, 0.0f };
    glm::vec3 scale { 1.0f };
    glm::vec3 translation { 0.0f };
};

struct Scene {
    std::vector<int> nodes;
};
}

class Model {
public:
    Model(const VK::MemoryAllocator* pMemoryAllocator, const VK::CommandPool* pCommandPool, const VK::Queue* pQueue);
    ~Model() = default;

public:
    void LoadFile(const std::string& filename);

public:
    void Render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);

private:
    // void ProcessNode(tinygltf::Model& model, int nodeIndex);
    // void ProcessMesh(tinygltf::Model& model, int meshIndex);

    void LoadScene(tinygltf::Model& tinyModel, size_t sceneIndex);
    void LoadNode(tinygltf::Model& tinyModel, size_t nodeIndex);
    void LoadMesh(tinygltf::Model& tinyModel, size_t meshIndex);
    void LoadPrimitive(tinygltf::Model& tinyModel, size_t meshIndex, tinygltf::Primitive& tinyPrimitive);
    void LoadMaterial(tinygltf::Model& tinyModel, size_t materialIndex);

    VK::Buffer CreateVertexBuffer(const std::vector<Vertex>& vertices);
    VK::Buffer CreateIndexBuffer(const std::vector<uint32_t>& indices);
    VK::Image CreateTexture(const std::string& path);

public:
    // std::vector<Mesh> m_meshes;
    std::filesystem::path m_filepath;
    std::vector<glTF::Scene> scenes;
    std::vector<glTF::Node> nodes;
    std::vector<glTF::Mesh> meshes;
    std::vector<glTF::Material> materials;
    int activeSceneIndex { -1 };

private:
    const VK::MemoryAllocator* p_memoryAllocator;
    const VK::CommandPool* p_commandPool;
    const VK::Queue* p_queue;
};
