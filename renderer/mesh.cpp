#include "pch.h"
#include "mesh.h"

Mesh::Mesh(const VK::Device* pDevice, const VK::MemoryAllocator* pAllocator, const VK::CommandPool* pCommandPool, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    : p_device { pDevice }
    , p_allocator { pAllocator }
    , p_commandPool { pCommandPool }
    , m_indexCount { static_cast<uint32_t>(indices.size()) }
{
    CreateVertexBuffer(vertices);
    CreateIndexBuffer(indices);
    CreateUniformBuffer();
}

Mesh::~Mesh()
{
    m_vertexBuffer.Destroy();
    m_indexBuffer.Destroy();
    m_uniformBuffer.Destroy();
}

void Mesh::Draw(const VkCommandBuffer& commandBuffer, const VkPipelineLayout& pipelineLayout)
{
    VkBuffer vertexBuffers[] = { m_vertexBuffer.GetHandle() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer.GetHandle(), 0, VK_INDEX_TYPE_UINT32);

    static float angle = 0.0f;
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
    angle += 0.01f;

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));

    m_uniformData.world = rotate * scale;
    m_uniformBuffer.CopyData(&m_uniformData, sizeof(MeshUniformData));

    VkDescriptorSet descriptorSets[] = { m_descriptorSet.GetHandle() };

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSets, 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
}

void Mesh::CreateVertexBuffer(const std::vector<Vertex>& vertices)
{
    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

    VkBufferUsageFlags stagingBufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateFlags stagingBufferAllocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    VK::Buffer stagingBuffer = p_allocator->CreateBuffer(bufferSize, stagingBufferUsage, stagingBufferAllocationFlags);
    stagingBuffer.CopyData((void*)vertices.data(), bufferSize);

    VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    m_vertexBuffer = p_allocator->CreateBuffer(bufferSize, bufferUsage);

    stagingBuffer.CopyToBuffer(p_commandPool->AllocateCommandBuffer(p_device->GetGrahpicsQueue()), m_vertexBuffer.GetHandle(), bufferSize);
}

void Mesh::CreateIndexBuffer(const std::vector<uint32_t>& indices)
{
    VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

    VkBufferUsageFlags stagingBufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateFlags stagingBufferAllocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    VK::Buffer stagingBuffer = p_allocator->CreateBuffer(bufferSize, stagingBufferUsage, stagingBufferAllocationFlags);
    stagingBuffer.CopyData((void*)indices.data(), bufferSize);

    VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    m_indexBuffer = p_allocator->CreateBuffer(bufferSize, bufferUsage);
    stagingBuffer.CopyToBuffer(p_commandPool->AllocateCommandBuffer(p_device->GetGrahpicsQueue()), m_indexBuffer.GetHandle(), bufferSize);
}

void Mesh::CreateUniformBuffer(void)
{
    VkDeviceSize bufferSize = sizeof(MeshUniformData);
    VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    VmaAllocationCreateFlags allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    m_uniformBuffer = p_allocator->CreateBuffer(bufferSize, bufferUsage, allocationFlags);
}
