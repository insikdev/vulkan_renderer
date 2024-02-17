#include "vk_instance.h"
#include "vk_physical_device.h"

VkResult VK::Instance::Init(const VkApplicationInfo* pApplicationInfo, const std::vector<const char*>& enabledLayerCount, const std::vector<const char*>& enabledExtensions)
{
    assert(m_handle == VK_NULL_HANDLE);

    VkInstanceCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO },
        .pNext {},
        .flags {},
        .pApplicationInfo { pApplicationInfo },
        .enabledLayerCount { static_cast<uint32_t>(enabledLayerCount.size()) },
        .ppEnabledLayerNames { enabledLayerCount.data() },
        .enabledExtensionCount { static_cast<uint32_t>(enabledExtensions.size()) },
        .ppEnabledExtensionNames { enabledExtensions.data() }
    };

    return vkCreateInstance(&createInfo, nullptr, &m_handle);
}

void VK::Instance::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyInstance(m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

std::vector<VK::PhysicalDevice> VK::Instance::GetPhysicalDevices(void) const
{
    assert(m_handle != VK_NULL_HANDLE);

    uint32_t count {};
    vkEnumeratePhysicalDevices(m_handle, &count, nullptr);

    std::vector<VkPhysicalDevice> vulkanPhysicalDevices(count);

    if (count != 0) {
        vkEnumeratePhysicalDevices(m_handle, &count, vulkanPhysicalDevices.data());
    }

    std::vector<VK::PhysicalDevice> physicalDevices;

    for (VkPhysicalDevice vulkanPhysicalDevice : vulkanPhysicalDevices) {
        physicalDevices.emplace_back().Init(vulkanPhysicalDevice);
    }

    return physicalDevices;
}
