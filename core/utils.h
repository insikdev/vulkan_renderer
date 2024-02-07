#pragma once

#include "common.h"

namespace Utils {
bool CheckLayerSupport(const std::vector<const char*>& requiredLayers, const std::vector<VkLayerProperties>& supportedLayers);
bool CheckExtensionSupport(const std::vector<const char*>& requiredExtensions, const std::vector<VkExtensionProperties>& supportedExtensions);
}
