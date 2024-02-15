#include "query.h"

std::vector<VkExtensionProperties> Query::GetInstanceExtensions(void)
{
    uint32_t count {};
    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> extensions(count);

    if (count != 0) {
        vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
    }

    return extensions;
}

std::vector<VkLayerProperties> Query::GetInstanceLayers(void)
{
    uint32_t count {};
    vkEnumerateInstanceLayerProperties(&count, nullptr);

    std::vector<VkLayerProperties> layers(count);

    if (count != 0) {
        vkEnumerateInstanceLayerProperties(&count, layers.data());
    }

    return layers;
}

std::vector<VkExtensionProperties> Query::GetDeviceExtensions(VkPhysicalDevice physicalDevice)
{
    uint32_t count {};
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> extensions(count);

    if (count != 0) {
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, extensions.data());
    }

    return extensions;
}
