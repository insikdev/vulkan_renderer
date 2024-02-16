#define VMA_IMPLEMENTATION
#include "vk_memory_allocator.h"
#include "vk_buffer.h"
#include "vk_image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "vk_command_pool.h"
#include "vk_command_buffer.h"

VkResult VK::MemoryAllocator::Init(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device)
{
    assert(m_handle == VK_NULL_HANDLE);

    VmaAllocatorCreateInfo createInfo {
        .physicalDevice { physicalDevice },
        .device { device },
        .instance { instance }
    };

    return vmaCreateAllocator(&createInfo, &m_handle);
}

void VK::MemoryAllocator::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vmaDestroyAllocator(m_handle);
        m_handle = VK_NULL_HANDLE;
    }
}

VK::Buffer VK::MemoryAllocator::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VmaAllocationCreateFlags allocationFlags) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VK::Buffer buffer;
    buffer.Init(m_handle, size, usageFlags, allocationFlags);

    return buffer;
}

VK::Buffer VK::MemoryAllocator::CreateBufferUsingStaging(const CommandBuffer& commandBuffer, VkDeviceSize bufferSize, VkBufferUsageFlags usageFlags, void* pData) const
{
    VK::Buffer stagingBuffer = CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
    stagingBuffer.CopyData(pData, bufferSize);

    VK::Buffer buffer = CreateBuffer(bufferSize, usageFlags);
    stagingBuffer.CopyToBuffer(commandBuffer, buffer.GetHandle(), bufferSize);

    return buffer;
}

VK::Image VK::MemoryAllocator::CreateImage(const VkExtent3D& extent3D, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VK::Image image;
    image.Init(m_handle, extent3D, format, tiling, usageFlags);

    return image;
}

VK::Image VK::MemoryAllocator::CreateTexture2D(const std::string& filepath, const CommandPool& commandPool, VkQueue queue) const
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    VkDeviceSize imageSize = texWidth * texHeight * 4;
    VkBufferUsageFlags stagingBufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateFlags stagingBufferAllocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    VK::Buffer stagingBuffer = CreateBuffer(imageSize, stagingBufferUsage, stagingBufferAllocationFlags);
    stagingBuffer.CopyData(pixels, imageSize);

    stbi_image_free(pixels);

    VkExtent3D extent3D = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 };

    VK::Image texture = CreateImage(extent3D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    texture.TransitionLayout(commandPool.AllocateCommandBuffer(queue), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    stagingBuffer.CopyToImage(commandPool.AllocateCommandBuffer(queue), texture.GetHandle(), extent3D);
    texture.TransitionLayout(commandPool.AllocateCommandBuffer(queue), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    return texture;
}
