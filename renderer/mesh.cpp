#include "pch.h"
#include "mesh.h"

Mesh::Mesh(VK::Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    : p_device { pDevice }
    , m_indexCount { static_cast<uint32_t>(indices.size()) }
{
    CreateVertexBuffer(vertices);
    CreateIndexBuffer(indices);
}

Mesh::~Mesh()
{
    delete m_vertexBuffer;
    delete m_indexBuffer;
}

void Mesh::Draw(VkCommandBuffer commandBuffer)
{
    VkBuffer vertexBuffers[] = { m_vertexBuffer->GetHandle() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
}

void Mesh::CreateVertexBuffer(const std::vector<Vertex>& vertices)
{
    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

    VkBufferUsageFlags stagingBufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateFlags stagingBufferAllocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    VK::Buffer* stagingBuffer = p_device->CreateBuffer(bufferSize, stagingBufferUsage, stagingBufferAllocationFlags);
    stagingBuffer->CopyDataToDevice((void*)vertices.data(), bufferSize);

    VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    m_vertexBuffer = p_device->CreateBuffer(bufferSize, bufferUsage);

    p_device->CopyBuffer(stagingBuffer->GetHandle(), m_vertexBuffer->GetHandle(), bufferSize);

    delete stagingBuffer;
}

void Mesh::CreateIndexBuffer(const std::vector<uint32_t>& indices)
{
    VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

    VkBufferUsageFlags stagingBufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateFlags stagingBufferAllocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    VK::Buffer* stagingBuffer = p_device->CreateBuffer(bufferSize, stagingBufferUsage, stagingBufferAllocationFlags);
    stagingBuffer->CopyDataToDevice((void*)indices.data(), bufferSize);

    VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    m_indexBuffer = p_device->CreateBuffer(bufferSize, bufferUsage);

    p_device->CopyBuffer(stagingBuffer->GetHandle(), m_indexBuffer->GetHandle(), bufferSize);

    delete stagingBuffer;
}
