#include "pch.h"
#include "query.h"

std::vector<VkPhysicalDevice> Query::GetPhysicalDevices(VkInstance instance)
{
    uint32_t deviceCount {};
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    std::vector<VkPhysicalDevice> devices(deviceCount);

    if (deviceCount != 0) {
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    }

    return devices;
}

std::vector<VkSurfaceFormatKHR> Query::GetSurfaceFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    uint32_t formatCount {};
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    std::vector<VkSurfaceFormatKHR> formats(formatCount);

    if (formatCount != 0) {
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
    }

    return formats;
}

std::vector<VkPresentModeKHR> Query::GetPresentModes(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    uint32_t presentModeCount {};
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);

    if (presentModeCount != 0) {
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
    }

    return presentModes;
}
