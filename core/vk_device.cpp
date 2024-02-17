#include "vk_device.h"
#include "vk_queue.h"

VkResult VK::Device::Init(VkPhysicalDevice physicalDevice, const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos, const std::vector<const char*>& enabledExtensions, const VkPhysicalDeviceFeatures* pEnabledFeatures)
{
    assert(m_handle == VK_NULL_HANDLE);

    VkDeviceCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO },
        .pNext {},
        .flags {},
        .queueCreateInfoCount { static_cast<uint32_t>(queueCreateInfos.size()) },
        .pQueueCreateInfos { queueCreateInfos.data() },
        .enabledLayerCount { /* deprecated */ },
        .ppEnabledLayerNames { /* deprecated */ },
        .enabledExtensionCount { static_cast<uint32_t>(enabledExtensions.size()) },
        .ppEnabledExtensionNames { enabledExtensions.data() },
        .pEnabledFeatures { pEnabledFeatures }
    };

    return vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_handle);
}

void VK::Device::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyDevice(m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VK::Queue VK::Device::GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VK::Queue queue;
    queue.Init(m_handle, queueFamilyIndex, queueIndex);

    return queue;
}

VkResult VK::Device::WaitIdle(void) const
{
    assert(m_handle != VK_NULL_HANDLE);

    return vkDeviceWaitIdle(m_handle);
}
