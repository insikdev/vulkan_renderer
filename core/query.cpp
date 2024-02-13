#include "query.h"

std::vector<VkExtensionProperties> Query::GetInstanceExtensions(void)
{
    uint32_t count {};
    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(count);

    if (count != 0) {
        vkEnumerateInstanceExtensionProperties(nullptr, &count, availableExtensions.data());
    }

    return availableExtensions;
}

std::vector<VkLayerProperties> Query::GetInstanceLayers(void)
{
    uint32_t count {};
    vkEnumerateInstanceLayerProperties(&count, nullptr);

    std::vector<VkLayerProperties> availableLayers(count);

    if (count != 0) {
        vkEnumerateInstanceLayerProperties(&count, availableLayers.data());
    }

    return availableLayers;
}

std::vector<VkPhysicalDevice> Query::GetPhysicalDevices(const VkInstance& instance)
{
    uint32_t count {};
    vkEnumeratePhysicalDevices(instance, &count, nullptr);

    std::vector<VkPhysicalDevice> availablePhysicalDevices(count);

    if (count != 0) {
        vkEnumeratePhysicalDevices(instance, &count, availablePhysicalDevices.data());
    }

    return availablePhysicalDevices;
}

std::vector<VkExtensionProperties> Query::GetDeviceExtensions(const VkPhysicalDevice& physicalDevice)
{
    uint32_t count {};
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(count);

    if (count != 0) {
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, availableExtensions.data());
    }

    return availableExtensions;
}

std::vector<VkQueueFamilyProperties> Query::GetQueueFamilies(const VkPhysicalDevice& physicalDevice)
{
    uint32_t count {};
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);

    std::vector<VkQueueFamilyProperties> availableQueueFamilies(count);

    if (count != 0) {
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, availableQueueFamilies.data());
    }

    return availableQueueFamilies;
}

std::vector<VkSurfaceFormatKHR> Query::GetSurfaceFormats(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
{
    uint32_t count {};
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);

    std::vector<VkSurfaceFormatKHR> availableFormats(count);

    if (count != 0) {
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, availableFormats.data());
    }

    return availableFormats;
}

std::vector<VkPresentModeKHR> Query::GetPresentModes(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
{
    uint32_t count {};
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);

    std::vector<VkPresentModeKHR> availablePresentModes(count);

    if (count != 0) {
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, availablePresentModes.data());
    }

    return availablePresentModes;
}
