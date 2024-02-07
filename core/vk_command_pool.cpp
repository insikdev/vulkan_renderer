#include "vk_command_pool.h"
#include "vk_command_buffer.h"

VK::CommandPool::~CommandPool()
{
    Destroy();
}

void VK::CommandPool::Initialize(const VkDevice& device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
    }

    VkCommandPoolCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO },
        .pNext { nullptr },
        .flags { createFlags },
        .queueFamilyIndex { queueFamilyIndex }
    };

    CHECK_VK(vkCreateCommandPool(m_device, &createInfo, nullptr, &m_handle), "Failed to create command pool.");
}

void VK::CommandPool::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VK::CommandBuffer VK::CommandPool::AllocateCommandBuffer(const VkQueue& queueToSubmit) const
{
    VK::CommandBuffer commandBuffer;
    commandBuffer.Initialize(m_device, m_handle, queueToSubmit);

    return commandBuffer;
}
