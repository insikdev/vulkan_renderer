#include "vk_fence.h"

VkResult VK::Fence::Init(VkDevice device, VkFenceCreateFlags createFlags)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
    }

    VkFenceCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO },
        .pNext { nullptr },
        .flags { createFlags }
    };

    return vkCreateFence(m_device, &createInfo, nullptr, &m_handle);
}

void VK::Fence::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyFence(m_device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
