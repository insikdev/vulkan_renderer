#pragma once

class VK::Device;

class Mesh {
public:
    Mesh(VK::Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    ~Mesh();

public:
    void Draw(VkCommandBuffer commandBuffer);

private:
    void CreateVertexBuffer(const std::vector<Vertex>& vertices);
    void CreateIndexBuffer(const std::vector<uint32_t>& indices);

public:
    VK::Device* p_device;
    VK::Buffer* m_vertexBuffer;
    VK::Buffer* m_indexBuffer;
    uint32_t m_indexCount;
};
