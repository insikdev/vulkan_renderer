#include "vk_semaphore.h"

void VK::Semaphore::Initialize(const VkDevice& device)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
    }

    VkSemaphoreCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO },
        .pNext { nullptr },
        .flags {}
    };

    CHECK_VK(vkCreateSemaphore(m_device, &createInfo, nullptr, &m_handle), "Failed to create semaphore.");
}

void VK::Semaphore::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroySemaphore(m_device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
