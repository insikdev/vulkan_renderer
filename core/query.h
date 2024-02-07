#pragma once

#include "common.h"

namespace Query {
// instance
std::vector<VkExtensionProperties> GetInstanceExtensions(void);
std::vector<VkLayerProperties> GetInstanceLayers(void);

// device
std::vector<VkPhysicalDevice> GetPhysicalDevices(const VkInstance& instance);
std::vector<VkExtensionProperties> GetDeviceExtensions(const VkPhysicalDevice& physicalDevice);
std::vector<VkQueueFamilyProperties> GetQueueFamilies(const VkPhysicalDevice& physicalDevice);

// swap chain
std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
std::vector<VkPresentModeKHR> GetPresentModes(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
}
