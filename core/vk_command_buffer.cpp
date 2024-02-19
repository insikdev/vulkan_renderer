#include "vk_command_buffer.h"

VK::CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
    : m_handle { other.m_handle }
    , m_device { other.m_device }
    , m_commandPool { other.m_commandPool }
{
    other.m_handle = VK_NULL_HANDLE;
}

VK::CommandBuffer& VK::CommandBuffer::operator=(CommandBuffer&& other) noexcept
{
    if (this != &other) {
        m_handle = other.m_handle;
        m_device = other.m_device;
        m_commandPool = other.m_commandPool;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

VkResult VK::CommandBuffer::Init(VkDevice device, VkCommandPool commandPool)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
        m_commandPool = commandPool;
    }

    VkCommandBufferAllocateInfo allocateInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },
        .pNext { nullptr },
        .commandPool { m_commandPool },
        .level { VK_COMMAND_BUFFER_LEVEL_PRIMARY },
        .commandBufferCount { 1 }
    };

    return vkAllocateCommandBuffers(m_device, &allocateInfo, &m_handle);
}

void VK::CommandBuffer::Free(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_handle);
        m_handle = VK_NULL_HANDLE;
    }
}

VkResult VK::CommandBuffer::Begin(VkCommandBufferUsageFlags usageFlags) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VkCommandBufferBeginInfo beginInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
        .pNext { nullptr },
        .flags { usageFlags },
        .pInheritanceInfo {}
    };

    return vkBeginCommandBuffer(m_handle, &beginInfo);
}

VkResult VK::CommandBuffer::End(void) const
{
    assert(m_handle != VK_NULL_HANDLE);

    return vkEndCommandBuffer(m_handle);
}

// CommandBuffer must have been allocated from a pool that was created with the VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
VkResult VK::CommandBuffer::Reset(VkCommandBufferResetFlags resetFlags) const
{
    assert(m_handle != VK_NULL_HANDLE);

    return vkResetCommandBuffer(m_handle, resetFlags);
}

void VK::CommandBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const
{
    VkBufferCopy copyRegion {
        .srcOffset {},
        .dstOffset {},
        .size { size }
    };

    vkCmdCopyBuffer(m_handle, srcBuffer, dstBuffer, 1, &copyRegion);
}
