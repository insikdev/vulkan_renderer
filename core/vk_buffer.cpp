#include "vk_buffer.h"
#include "vk_command_buffer.h"

VK::Buffer::~Buffer()
{
    Destroy();
}

VK::Buffer::Buffer(Buffer&& other) noexcept
    : m_handle { other.m_handle }
    , m_allocation { other.m_allocation }
    , m_allocator { other.m_allocator }
{
    other.m_handle = VK_NULL_HANDLE;
}

VK::Buffer& VK::Buffer::operator=(Buffer&& other) noexcept
{
    if (this != &other) {
        m_handle = other.m_handle;
        m_allocation = other.m_allocation;
        m_allocator = other.m_allocator;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

void VK::Buffer::Initialize(const VmaAllocator& allocator, VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_allocator = allocator;
    }

    VkBufferCreateInfo bufferCreateInfo {
        .sType { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .size { size },
        .usage { usage },
        .sharingMode {},
        .queueFamilyIndexCount {},
        .pQueueFamilyIndices {}
    };

    VmaAllocationCreateInfo allocationCreateInfo = {
        .flags { allocationFlags },
        .usage { VMA_MEMORY_USAGE_AUTO },
        .requiredFlags {},
        .preferredFlags {},
        .memoryTypeBits {},
        .pool {},
        .pUserData {},
        .priority {}
    };

    CHECK_VK(vmaCreateBuffer(m_allocator, &bufferCreateInfo, &allocationCreateInfo, &m_handle, &m_allocation, nullptr), "Failed to create buffer.");
}

void VK::Buffer::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vmaDestroyBuffer(m_allocator, m_handle, m_allocation);
        m_handle = VK_NULL_HANDLE;
    }
}

void VK::Buffer::CopyData(void* pSrc, VkDeviceSize size)
{
    assert(m_allocation != VK_NULL_HANDLE);

    void* pData;
    vmaMapMemory(m_allocator, m_allocation, &pData);
    memcpy(pData, pSrc, size);
    vmaUnmapMemory(m_allocator, m_allocation);
}

void VK::Buffer::CopyToBuffer(const CommandBuffer& commandBuffer, const VkBuffer& dstBuffer, VkDeviceSize size)
{
    commandBuffer.BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferCopy copyRegion {
        .srcOffset {},
        .dstOffset {},
        .size { size }
    };

    vkCmdCopyBuffer(commandBuffer.GetHandle(), m_handle, dstBuffer, 1, &copyRegion);

    commandBuffer.EndRecording();
    commandBuffer.Submit();
}

void VK::Buffer::CopyToImage(const CommandBuffer& commandBuffer, const VkImage& image, const VkExtent3D& extent3D)
{
    commandBuffer.BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkImageSubresourceLayers imageSubresource {
        .aspectMask { VK_IMAGE_ASPECT_COLOR_BIT },
        .mipLevel { 0 },
        .baseArrayLayer { 0 },
        .layerCount { 1 }
    };

    VkBufferImageCopy region {
        .bufferOffset { 0 },
        .bufferRowLength { 0 },
        .bufferImageHeight { 0 },
        .imageSubresource { imageSubresource },
        .imageOffset { 0, 0, 0 },
        .imageExtent { extent3D }
    };

    vkCmdCopyBufferToImage(commandBuffer.GetHandle(), m_handle, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    commandBuffer.EndRecording();
    commandBuffer.Submit();
}
