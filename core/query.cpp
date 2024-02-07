#include "query.h"

std::vector<VkExtensionProperties> Query::GetInstanceExtensions(void)
{
    uint32_t extensionCount {};
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);

    if (extensionCount != 0) {
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
    }

    return availableExtensions;
}

std::vector<VkLayerProperties> Query::GetInstanceLayers(void)
{
    uint32_t layerCount {};
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);

    if (layerCount != 0) {
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    }

    return availableLayers;
}

std::vector<VkPhysicalDevice> Query::GetPhysicalDevices(const VkInstance& instance)
{
    uint32_t deviceCount {};
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    std::vector<VkPhysicalDevice> availablePhysicalDevices(deviceCount);

    if (deviceCount != 0) {
        vkEnumeratePhysicalDevices(instance, &deviceCount, availablePhysicalDevices.data());
    }

    return availablePhysicalDevices;
}

std::vector<VkExtensionProperties> Query::GetDeviceExtensions(const VkPhysicalDevice& physicalDevice)
{
    uint32_t extensionCount {};
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);

    if (extensionCount != 0) {
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());
    }

    return availableExtensions;
}

std::vector<VkQueueFamilyProperties> Query::GetQueueFamilies(const VkPhysicalDevice& physicalDevice)
{
    uint32_t queueFamilyCount {};
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> availableQueueFamilies(queueFamilyCount);

    if (queueFamilyCount != 0) {
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, availableQueueFamilies.data());
    }

    return availableQueueFamilies;
}

std::vector<VkSurfaceFormatKHR> Query::GetSurfaceFormats(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
{
    uint32_t formatCount {};
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    std::vector<VkSurfaceFormatKHR> availableFormats(formatCount);

    if (formatCount != 0) {
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, availableFormats.data());
    }

    return availableFormats;
}

std::vector<VkPresentModeKHR> Query::GetPresentModes(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
{
    uint32_t presentModeCount {};
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    std::vector<VkPresentModeKHR> availablePresentModes(presentModeCount);

    if (presentModeCount != 0) {
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, availablePresentModes.data());
    }

    return availablePresentModes;
}
