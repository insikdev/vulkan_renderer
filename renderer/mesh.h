#pragma once

class Mesh {
public:
    Mesh(const VK::Device* pDevice, const VK::MemoryAllocator* pAllocator, const VK::CommandPool* pCommandPool, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    ~Mesh();

public:
    void Draw(const VkCommandBuffer& commandBuffer);

public: // getter
    VkBuffer GetUniformBuffer(void) const { return m_uniformBuffer.GetHandle(); }

private:
    void CreateVertexBuffer(const std::vector<Vertex>& vertices);
    void CreateIndexBuffer(const std::vector<uint32_t>& indices);
    void CreateUniformBuffer(void);

private:
    const VK::Device* p_device { nullptr };
    const VK::MemoryAllocator* p_allocator { nullptr };
    const VK::CommandPool* p_commandPool { nullptr };

public:
    VK::Buffer m_vertexBuffer;
    VK::Buffer m_indexBuffer;
    VK::Buffer m_uniformBuffer;
    MeshUniformData m_uniformData;
    uint32_t m_indexCount;
};
