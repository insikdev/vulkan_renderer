#include "pch.h"
#include "mesh.h"
#include "image.h"

void Mesh::Init(const VK::MemoryAllocator& memoryAllocator, const VK::CommandBuffer& commandBuffer, const VK::Queue& queue, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& texturePath)
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

#pragma region create uniform buffer
    m_uniformBuffer = memoryAllocator.CreateBuffer(
        sizeof(MeshUniformData),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
#pragma endregion

#pragma region create diffuse texture
    ImageFile image;
    image.Init(texturePath);

    VkBufferUsageFlags stagingBufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateFlags stagingBufferAllocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    VK::Buffer stagingBuffer = memoryAllocator.CreateBuffer(image.GetSize(), stagingBufferUsage, stagingBufferAllocationFlags);
    stagingBuffer.CopyData(image.GetData(), image.GetSize());

    VkExtent3D extent3D = { static_cast<uint32_t>(image.GetWidth()), static_cast<uint32_t>(image.GetHeight()), 1 };

    m_uniformData.diffuseImage = memoryAllocator.CreateImage(extent3D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    m_uniformData.diffuseImage.TransitionLayout(commandBuffer,
        VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

    stagingBuffer.CopyToImage(commandBuffer, m_uniformData.diffuseImage.GetHandle(), extent3D);

    m_uniformData.diffuseImage.TransitionLayout(commandBuffer,
        VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

#pragma endregion

    commandBuffer.End();
    queue.Submit({ commandBuffer.GetHandle() });
    queue.WaitIdle();

    m_indexCount = static_cast<uint32_t>(indices.size());
}

void Mesh::Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
    VkBuffer vertexBuffers[] = { m_vertexBuffer.GetHandle() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer.GetHandle(), 0, VK_INDEX_TYPE_UINT32);

    m_uniformData.world = transform.GetWorldMatrix();
    m_uniformBuffer.CopyData(&m_uniformData, sizeof(MeshUniformData));

    VkDescriptorSet descriptorSets[] = { m_descriptorSet.GetHandle() };

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSets, 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
}
