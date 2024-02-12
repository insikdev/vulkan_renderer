#pragma once

#include "tiny_gltf.h"
class Mesh;

class Model {
public:
    Model() = default;
    ~Model();

public:
    void Render(const VkCommandBuffer& commandBuffer, const VkPipelineLayout& pipelineLayout);
    void ReadFromFile(const std ::string& filename, const VK::Device* pDevice, const VK::MemoryAllocator* pAllocator, const VK::CommandPool* pCommandPool);

private:
    void ProcessNode(tinygltf::Model& model, tinygltf::Node& currentNode, const VK::Device* pDevice, const VK::MemoryAllocator* pAllocator, const VK::CommandPool* pCommandPool);
    void ProcessMesh(tinygltf::Model& model, tinygltf::Mesh& currentMesh, const VK::Device* pDevice, const VK::MemoryAllocator* pAllocator, const VK::CommandPool* pCommandPool);

public:
    std::vector<Mesh*> m_meshes;
};
