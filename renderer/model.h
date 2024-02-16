#pragma once

#include <tiny_gltf.h>
#include "mesh.h"

class Model {
public:
    void Init(const std::string& filename, const VK::Device* pDevice, const VK::MemoryAllocator* pAllocator, const VK::CommandPool* pCommandPool);

public:
    void Render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);

private:
    void ProcessNode(tinygltf::Model& model, tinygltf::Node& currentNode, const VK::Device* pDevice, const VK::MemoryAllocator* pAllocator, const VK::CommandPool* pCommandPool);
    void ProcessMesh(tinygltf::Model& model, tinygltf::Mesh& currentMesh, const VK::Device* pDevice, const VK::MemoryAllocator* pAllocator, const VK::CommandPool* pCommandPool);

public:
    std::vector<Mesh> m_meshes;
    std::filesystem::path m_filepath;
};
