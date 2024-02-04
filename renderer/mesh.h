#pragma once

class VK::Device;

class Mesh {
public:
    Mesh(VK::Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    ~Mesh();

public:
    void Draw(VkCommandBuffer commandBuffer);

public: // getter
    VkBuffer GetUniformBuffer(void) const { return m_uniformBuffer.handle; }

private:
    void CreateVertexBuffer(const std::vector<Vertex>& vertices);
    void CreateIndexBuffer(const std::vector<uint32_t>& indices);
    void CreateUniformBuffer(void);

public:
    VK::Device* p_device;
    VK::Buffer m_vertexBuffer;
    VK::Buffer m_indexBuffer;
    VK::Buffer m_uniformBuffer;
    MeshUniformData m_uniformData;
    uint32_t m_indexCount;
};
