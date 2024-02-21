#pragma once

class Mesh {
public:
    void Init(
        const VK::MemoryAllocator& memoryAllocator,
        const VK::CommandBuffer& commandBuffer,
        const VK::Queue& queue,
        const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices,
        const std::string& texturePath);

public:
    void Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);

public:
    VK::Buffer m_vertexBuffer;
    VK::Buffer m_indexBuffer;
    VK::Buffer m_uniformBuffer;
    VK::DescriptorSet m_descriptorSet;
    MeshUniformData m_uniformData {};
    uint32_t m_indexCount {};
    Graphics::Transform transform;
};
