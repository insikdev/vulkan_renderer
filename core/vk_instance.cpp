#include "vk_instance.h"

void VK::Instance::Initialize(const VkApplicationInfo* pApplicationInfo, const std::vector<const char*>& requiredLayers, const std::vector<const char*>& requiredExtensions)
{
    assert(m_handle == VK_NULL_HANDLE);

    VkInstanceCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .pApplicationInfo { pApplicationInfo },
        .enabledLayerCount { static_cast<uint32_t>(requiredLayers.size()) },
        .ppEnabledLayerNames { requiredLayers.data() },
        .enabledExtensionCount { static_cast<uint32_t>(requiredExtensions.size()) },
        .ppEnabledExtensionNames { requiredExtensions.data() }
    };

    CHECK_VK(vkCreateInstance(&createInfo, nullptr, &m_handle), "Failed to create instance.");
}

void VK::Instance::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyInstance(m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
