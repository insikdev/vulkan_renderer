#include "pch.h"
#include "glTF_primitive.h"

void glTF::Primitive::Init(const VK::MemoryAllocator& memoryAllocator, const VK::CommandBuffer& commandBuffer, const VK::Queue& queue, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
    commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

#pragma region create vertex buffer
    VkDeviceSize vertexBufferSize = sizeof(Vertex) * vertices.size();

    VK::Buffer vertexStagingBuffer = memoryAllocator.CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
    vertexStagingBuffer.CopyData((void*)vertices.data(), vertexBufferSize);

    m_vertexBuffer = memoryAllocator.CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    vertexStagingBuffer.CopyToBuffer(commandBuffer, m_vertexBuffer.GetHandle(), vertexBufferSize);
#pragma endregion

#pragma region create index buffer
    VkDeviceSize indexBufferSize = sizeof(uint32_t) * indices.size();

    VK::Buffer indexStagingBuffer = memoryAllocator.CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
    indexStagingBuffer.CopyData((void*)indices.data(), indexBufferSize);

    m_indexBuffer = memoryAllocator.CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    indexStagingBuffer.CopyToBuffer(commandBuffer, m_indexBuffer.GetHandle(), indexBufferSize);
#pragma endregion

    commandBuffer.End();
    queue.Submit({ commandBuffer.GetHandle() });
    queue.WaitIdle();

    m_indexCount = static_cast<uint32_t>(indices.size());
}

void glTF::Primitive::Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
    VkBuffer vertexBuffers[] = { m_vertexBuffer.GetHandle() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer.GetHandle(), 0, VK_INDEX_TYPE_UINT32);

    // m_uniformData.world = transform.GetWorldMatrix();
    // m_uniformBuffer.CopyData(&m_uniformData, sizeof(MeshUniformData));

    // VkDescriptorSet descriptorSets[] = { m_descriptorSet.GetHandle() };

    // vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSets, 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
}
