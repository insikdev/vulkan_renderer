#pragma once

#include "common.h"

namespace Query {
std::vector<VkExtensionProperties> GetInstanceExtensions(void);
std::vector<VkLayerProperties> GetInstanceLayers(void);
std::vector<VkExtensionProperties> GetDeviceExtensions(VkPhysicalDevice physicalDevice);
}
