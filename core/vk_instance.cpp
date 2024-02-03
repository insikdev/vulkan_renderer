#include "pch.h"
#include "vk_instance.h"
#include "query.h"
#include "utils.h"

VK::Instance::Instance(const std::vector<const char*>& requiredLayers, const std::vector<const char*>& requiredExtensions)
{
    if (Utils::CheckLayerSupport(requiredLayers, Query::GetInstanceLayers()) == false) {
        throw std::runtime_error("Required layers are not supported.");
    }

    if (Utils::CheckExtensionSupport(requiredExtensions, Query::GetInstanceExtensions()) == false) {
        throw std::runtime_error("Required extensions are not supported.");
    }

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
        .enabledLayerCount { static_cast<uint32_t>(requiredLayers.size()) },
        .ppEnabledLayerNames { requiredLayers.data() },
        .enabledExtensionCount { static_cast<uint32_t>(requiredExtensions.size()) },
        .ppEnabledExtensionNames { requiredExtensions.data() }
    };

    CHECK_VK(vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance), "Failed to create instance.");
}

VK::Instance::~Instance()
{
    vkDestroyInstance(m_instance, nullptr);
}
