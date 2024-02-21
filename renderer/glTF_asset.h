#pragma once

#include <tiny_gltf.h>

namespace glTF {
struct MaterialData {
    glm::vec4 baseColorFactor;
    float metallicFactor;
    float roughnessFactor;
    float dummy[2];
};

struct Material {
    std::optional<int> baseColorTextureIndex { std::nullopt };
    std::optional<int> metallicRoughnessTextureIndex { std::nullopt };

    MaterialData uniformData;
    VK::Buffer uniformBuffer;
    VK::DescriptorSet descriptorSet;
};

struct Texture {
    VK::Image texture;
    VK::ImageView textureView;
};

struct Primitive {
    VK::Buffer vertexBuffer;
    VK::Buffer indexBuffer;
    uint32_t indexCount {};
    std::optional<int> materialIndex { std::nullopt };
};

struct Mesh {
    std::vector<Primitive> primitives;
};

struct Node {
    std::optional<int> parentIndex { std::nullopt };
    std::optional<int> cameraIndex { std::nullopt };
    std::optional<int> skinIndex { std::nullopt };
    std::optional<int> meshIndex { std::nullopt };
    std::vector<int> children;
    glm::mat4 matrix;
    glm::quat rotation;
    glm::vec3 scale;
    glm::vec3 translation;
    VK::Buffer uniformBuffer;
    VK::DescriptorSet descriptorSet;
};

struct Scene {
    std::vector<int> nodes;
};
}

namespace glTF {
class Asset {
public:
    Asset(const VK::MemoryAllocator* pMemoryAllocator, const VK::CommandPool* pCommandPool, const VK::Queue* pQueue);
    ~Asset() = default;

public:
    void LoadAsset(const std::string& filename);

public:
    void Render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
    void RenderNode(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t nodeIndex);

private:
    void LoadScene(tinygltf::Model& tinyModel, size_t sceneIndex);
    void LoadNode(tinygltf::Model& tinyModel, size_t nodeIndex);
    void LoadMesh(tinygltf::Model& tinyModel, size_t meshIndex);
    void LoadPrimitive(tinygltf::Model& tinyModel, size_t meshIndex, size_t primitiveIndex);
    void LoadTexture(tinygltf::Model& tinyModel, size_t textureIndex);
    void LoadMaterial(tinygltf::Model& tinyModel, size_t materialIndex);

    VK::Buffer CreateVertexBuffer(const std::vector<Vertex>& vertices);
    VK::Buffer CreateIndexBuffer(const std::vector<uint32_t>& indices);
    VK::Image CreateTexture(tinygltf::Image& tinyImage);
    glm::mat4 GetNodeMatrix(size_t nodeIndex);

public:
    std::vector<glTF::Scene> scenes;
    std::vector<glTF::Node> nodes;
    std::vector<glTF::Mesh> meshes;
    std::vector<glTF::Texture> textures;
    std::vector<glTF::Material> materials;
    int activeSceneIndex { -1 };
    VK::Image defaultTexture;
    VK::ImageView defaultTextureView;

private:
    const VK::MemoryAllocator* p_memoryAllocator;
    const VK::CommandPool* p_commandPool;
    const VK::Queue* p_queue;
};
}
