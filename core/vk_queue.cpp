#include "vk_queue.h"

VK::Queue::Queue(Queue&& other) noexcept
    : m_handle { other.m_handle }
    , m_queueFamilyIndex { other.m_queueFamilyIndex }
    , m_queueIndex { other.m_queueIndex }
{
    other.m_handle = VK_NULL_HANDLE;
}

VK::Queue& VK::Queue::operator=(Queue&& other) noexcept
{
    if (this != &other) {
        m_handle = other.m_handle;
        m_queueFamilyIndex = other.m_queueFamilyIndex;
        m_queueIndex = other.m_queueIndex;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

void VK::Queue::Init(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_queueFamilyIndex = queueFamilyIndex;
        m_queueIndex = queueIndex;
    }

    vkGetDeviceQueue(device, m_queueFamilyIndex, m_queueIndex, &m_handle);
}
