#include "pch.h"
#include "vk_command_pool.h"
#include "vk_device.h"
#include "vk_command_buffer.h"
#include "check_vk.h"

VK::CommandPool::~CommandPool()
{
    Destroy();
}

void VK::CommandPool::Initialize(const Device* pDevice, VkCommandPoolCreateFlags createFlags)
{
    assert(m_handle == VK_NULL_HANDLE && pDevice != nullptr);

    {
        p_device = pDevice;
    }

    VkCommandPoolCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO },
        .pNext { nullptr },
        .flags { createFlags },
        .queueFamilyIndex { p_device->GetGraphicQueueFamilyIndex() }
    };

    CHECK_VK(vkCreateCommandPool(p_device->GetHandle(), &createInfo, nullptr, &m_handle), "Failed to create command pool.");
}

void VK::CommandPool::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyCommandPool(p_device->GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
        p_device = nullptr;
    }
}

VK::CommandBuffer VK::CommandPool::AllocateCommandBuffer(void) const
{
    VK::CommandBuffer commandBuffer;
    commandBuffer.Initialize(p_device, this);

    return commandBuffer;
}
