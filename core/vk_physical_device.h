#pragma once

#include "common.h"

namespace VK {
class PhysicalDevice {
public:
    // PhysicalDevice() = default;
    //~PhysicalDevice() = default;
    // PhysicalDevice(const PhysicalDevice&) = delete;
    // PhysicalDevice(PhysicalDevice&&) = default;
    // PhysicalDevice& operator=(const PhysicalDevice&) = delete;
    // PhysicalDevice& operator=(PhysicalDevice&&) = default;

public:
    void Init(VkPhysicalDevice physicalDevice);

public:
    VkPhysicalDevice GetHandle(void) const { return m_handle; }
    VkPhysicalDeviceProperties GetProperties(void) const;
    VkPhysicalDeviceFeatures GetFeatures(void) const;
    std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties(void) const;
    VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VkSurfaceKHR surface) const;
    std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(VkSurfaceKHR surface) const;
    VkFormatProperties GetFormatProperties(VkFormat format) const;
    bool CheckPresentationSupport(uint32_t queueFamilyIndex, VkSurfaceKHR surface) const;

private:
    VkPhysicalDevice m_handle { VK_NULL_HANDLE };

    // VkPhysicalDeviceProperties2 m_properties0 { .sType { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 }, .pNext { &m_properties1 } };
    // VkPhysicalDeviceVulkan11Properties m_properties1 { .sType { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES }, .pNext { &m_properties2 } };
    // VkPhysicalDeviceVulkan12Properties m_properties2 { .sType { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES }, .pNext { &m_properties3 } };
    // VkPhysicalDeviceVulkan13Properties m_properties3 { .sType { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES } };
    // VkPhysicalDeviceFeatures2 m_features0 { .sType { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 }, .pNext { &m_features1 } };
    // VkPhysicalDeviceVulkan11Features m_features1 { .sType { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES }, .pNext { &m_features2 } };
    // VkPhysicalDeviceVulkan12Features m_features2 { .sType { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES }, .pNext { &m_features3 } };
    // VkPhysicalDeviceVulkan13Features m_features3 { .sType { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES } };
};
}
