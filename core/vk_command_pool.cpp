#include "vk_command_pool.h"
#include "vk_command_buffer.h"

VK::CommandPool::CommandPool(CommandPool&& other) noexcept
    : m_handle { other.m_handle }
    , m_device { other.m_device }
{
    other.m_handle = VK_NULL_HANDLE;
}

VK::CommandPool& VK::CommandPool::operator=(CommandPool&& other) noexcept
{
    if (this != &other) {
        m_handle = other.m_handle;
        m_device = other.m_device;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

VkResult VK::CommandPool::Init(VkDevice device, VkCommandPoolCreateFlags createFlags, uint32_t queueFamilyIndex)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
    }

    VkCommandPoolCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO },
        .pNext {},
        .flags { createFlags },
        .queueFamilyIndex { queueFamilyIndex }
    };

    return vkCreateCommandPool(m_device, &createInfo, nullptr, &m_handle);
}

void VK::CommandPool::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VK::CommandBuffer VK::CommandPool::AllocateCommandBuffer(void) const
{
    VK::CommandBuffer commandBuffer;
    commandBuffer.Init(m_device, m_handle);

    return commandBuffer;
}
