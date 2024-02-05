#include "pch.h"
#include "vk_command_pool.h"
#include "vk_command_buffer.h"
#include "check_vk.h"

VK::CommandPool::~CommandPool()
{
    Destroy();
}

void VK::CommandPool::Initialize(VkDevice device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags)
{
    assert(m_handle == VK_NULL_HANDLE && device != VK_NULL_HANDLE);

    m_device = device;

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

VK::CommandBuffer VK::CommandPool::AllocateCommandBuffer() const
{
    assert(m_handle != VK_NULL_HANDLE);

    VK::CommandBuffer commandBuffer {};
    commandBuffer.Initialize(m_device, m_handle);

    return commandBuffer;
}

std::unique_ptr<VK::CommandBuffer> VK::CommandPool::AllocateCommandBufferUPTR() const
{
    assert(m_handle != VK_NULL_HANDLE);

    std::unique_ptr<VK::CommandBuffer> commandBuffer = std::make_unique<VK::CommandBuffer>();
    commandBuffer->Initialize(m_device, m_handle);

    return commandBuffer;
}
