#include "pch.h"
#include "mesh.h"

void Mesh::Init(const VK::MemoryAllocator* pAllocator, const VK::CommandPool* pCommandPool, VkQueue queue, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
    m_vertexBuffer = pAllocator->CreateBufferUsingStaging(
        pCommandPool->AllocateCommandBuffer(queue),
        sizeof(Vertex) * vertices.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        (void*)vertices.data());

    m_indexBuffer = pAllocator->CreateBufferUsingStaging(
        pCommandPool->AllocateCommandBuffer(queue),
        sizeof(uint32_t) * indices.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        (void*)indices.data());

    m_uniformBuffer = pAllocator->CreateBuffer(
        sizeof(MeshUniformData),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);

    m_indexCount = static_cast<uint32_t>(indices.size());
}

void Mesh::Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
    VkBuffer vertexBuffers[] = { m_vertexBuffer.GetHandle() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer.GetHandle(), 0, VK_INDEX_TYPE_UINT32);

    static float angle = 0.0f;
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
    angle += 0.01f;

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f));

    m_uniformData.world = rotate * scale;
    m_uniformBuffer.CopyData(&m_uniformData, sizeof(MeshUniformData));

    VkDescriptorSet descriptorSets[] = { m_descriptorSet.GetHandle() };

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSets, 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
}
