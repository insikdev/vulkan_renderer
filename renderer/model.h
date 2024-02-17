#pragma once

#include <tiny_gltf.h>
#include "mesh.h"

class Model {
public:
    void Init(const std::string& filename, const VK::Device& device, const VK::MemoryAllocator& memoryAllocator, const VK::CommandPool& commandPool, const VK::Queue& queue);

public:
    void Render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);

private:
    void ProcessNode(tinygltf::Model& model, tinygltf::Node& currentNode, const VK::Device& device, const VK::MemoryAllocator& memoryAllocator, const VK::CommandPool& commandPool, const VK::Queue& queue);
    void ProcessMesh(tinygltf::Model& model, tinygltf::Mesh& currentMesh, const VK::Device& device, const VK::MemoryAllocator& memoryAllocator, const VK::CommandPool& commandPool, const VK::Queue& queue);

public:
    std::vector<Mesh> m_meshes;
    std::filesystem::path m_filepath;
};
