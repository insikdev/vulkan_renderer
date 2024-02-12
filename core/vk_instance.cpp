#include "vk_instance.h"
#include "query.h"

void VK::Instance::Initialize(const std::vector<const char*>& requiredLayers, const std::vector<const char*>& requiredExtensions)
{
    assert(m_handle == VK_NULL_HANDLE);

    VkApplicationInfo applicationInfo {
        .sType { VK_STRUCTURE_TYPE_APPLICATION_INFO },
        .pNext { nullptr },
        .pApplicationName { nullptr },
        .applicationVersion { VK_MAKE_VERSION(0, 0, 1) },
        .pEngineName { nullptr },
        .engineVersion { VK_MAKE_VERSION(0, 0, 1) },
        .apiVersion { VK_API_VERSION_1_0 }
    };

    VkInstanceCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .pApplicationInfo { &applicationInfo },
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
