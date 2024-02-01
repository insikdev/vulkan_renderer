#include "pch.h"
#include "vk_instance.h"

VK::Instance::Instance(const std::vector<const char*>& layers, const std::vector<const char*>& extensions)
{
    CreateInstance(layers, extensions);
}

VK::Instance::~Instance()
{
    vkDestroyInstance(m_instance, nullptr);
}

void VK::Instance::CreateInstance(const std::vector<const char*>& layers, const std::vector<const char*>& extensions)
{
    VkApplicationInfo applicationInfo {
        .sType { VK_STRUCTURE_TYPE_APPLICATION_INFO },
        .pNext { nullptr },
        .pApplicationName { nullptr },
        .applicationVersion { VK_MAKE_VERSION(0, 0, 1) },
        .pEngineName { nullptr },
        .engineVersion { VK_MAKE_VERSION(0, 0, 1) },
        .apiVersion { VK_API_VERSION_1_0 }
    };

    VkInstanceCreateInfo instanceCreateInfo {
        .sType { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .pApplicationInfo { &applicationInfo },
        .enabledLayerCount { static_cast<uint32_t>(layers.size()) },
        .ppEnabledLayerNames { layers.data() },
        .enabledExtensionCount { static_cast<uint32_t>(extensions.size()) },
        .ppEnabledExtensionNames { extensions.data() }
    };

    CHECK_VK(vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance), "Failed to create instance.");
}
