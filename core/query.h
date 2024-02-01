#pragma once

namespace Query {
std::vector<VkPhysicalDevice> GetPhysicalDevices(VkInstance instance);
std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
std::vector<VkPresentModeKHR> GetPresentModes(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

}
