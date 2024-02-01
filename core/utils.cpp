#include "pch.h"
#include "utils.h"

bool Utils::CheckLayerSupport(const std::vector<const char*>& requiredLayers, const std::vector<VkLayerProperties>& supportedLayers)
{
    for (const auto& layer : requiredLayers) {
        bool found = false;

        for (const auto& supportedLayer : supportedLayers) {
            if (std::strcmp(layer, supportedLayer.layerName) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            return false;
        }
    }

    return true;
}

bool Utils::CheckExtensionSupport(const std::vector<const char*>& requiredExtensions, const std::vector<VkExtensionProperties>& supportedExtensions)
{
    for (const auto& extension : requiredExtensions) {
        bool found = false;

        for (const auto& supportedExtension : supportedExtensions) {
            if (std::strcmp(extension, supportedExtension.extensionName) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            return false;
        }
    }

    return true;
}
