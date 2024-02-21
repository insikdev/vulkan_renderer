#include "vk_physical_device.h"

void VK::PhysicalDevice::Init(VkPhysicalDevice physicalDevice)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_handle = physicalDevice;
    }
    // vkGetPhysicalDeviceProperties2(m_handle, &m_properties0);
    // vkGetPhysicalDeviceFeatures2(m_handle, &m_features0);
}

VkPhysicalDeviceProperties VK::PhysicalDevice::GetProperties(void) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VkPhysicalDeviceProperties physicalDeviceProperties {};
    vkGetPhysicalDeviceProperties(m_handle, &physicalDeviceProperties);

    return physicalDeviceProperties;
}

VkPhysicalDeviceFeatures VK::PhysicalDevice::GetFeatures(void) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VkPhysicalDeviceFeatures physicalDeviceFeatures {};
    vkGetPhysicalDeviceFeatures(m_handle, &physicalDeviceFeatures);

    return physicalDeviceFeatures;
}

std::vector<VkQueueFamilyProperties> VK::PhysicalDevice::GetQueueFamilyProperties(void) const
{
    assert(m_handle != VK_NULL_HANDLE);

    uint32_t count {};
    vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &count, nullptr);

    std::vector<VkQueueFamilyProperties> availableQueueFamilies(count);

    if (count != 0) {
        vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &count, availableQueueFamilies.data());
    }

    return availableQueueFamilies;
}

VkSurfaceCapabilitiesKHR VK::PhysicalDevice::GetSurfaceCapabilities(VkSurfaceKHR surface) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VkSurfaceCapabilitiesKHR surfaceCapabilities {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_handle, surface, &surfaceCapabilities);

    return surfaceCapabilities;
}

std::vector<VkSurfaceFormatKHR> VK::PhysicalDevice::GetSurfaceFormats(VkSurfaceKHR surface) const
{
    assert(m_handle != VK_NULL_HANDLE);

    uint32_t count {};
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_handle, surface, &count, nullptr);

    std::vector<VkSurfaceFormatKHR> availableFormats(count);

    if (count != 0) {
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_handle, surface, &count, availableFormats.data());
    }

    return availableFormats;
}

VkFormatProperties VK::PhysicalDevice::GetFormatProperties(VkFormat format) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(m_handle, format, &properties);

    return properties;
}

bool VK::PhysicalDevice::CheckPresentationSupport(uint32_t queueFamilyIndex, VkSurfaceKHR surface) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VkBool32 presentSupport {};
    vkGetPhysicalDeviceSurfaceSupportKHR(m_handle, queueFamilyIndex, surface, &presentSupport);

    return presentSupport;
}
