#pragma once

#include "common.h"

namespace Query {
// instance
std::vector<VkExtensionProperties> GetInstanceExtensions(void);
std::vector<VkLayerProperties> GetInstanceLayers(void);

// device
std::vector<VkPhysicalDevice> GetPhysicalDevices(VkInstance instance);
std::vector<VkExtensionProperties> GetDeviceExtensions(VkPhysicalDevice physicalDevice);
std::vector<VkQueueFamilyProperties> GetQueueFamilies(VkPhysicalDevice physicalDevice);

// swap chain
std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
std::vector<VkPresentModeKHR> GetPresentModes(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
}
