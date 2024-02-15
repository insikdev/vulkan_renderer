#include "vk_semaphore.h"

VkResult VK::Semaphore::Init(VkDevice device)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
    }

    VkSemaphoreCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO },
        .pNext {},
        .flags {}
    };

    return vkCreateSemaphore(m_device, &createInfo, nullptr, &m_handle);
}

void VK::Semaphore::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroySemaphore(m_device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
