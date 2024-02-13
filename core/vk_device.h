#pragma once

#include "common.h"

namespace VK {
class CommandPool;
class DescriptorPool;

class Device {
public:
    Device() = default;
    ~Device() { Destroy(); }
    Device(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) = delete;

public:
    void Initialize(const VkInstance& instance, const VkSurfaceKHR& surface, const std::vector<const char*>& requiredExtensions);
    void Destroy(void);

public: // method
    CommandPool CreateCommandPool(VkCommandPoolCreateFlags createFlags = 0) const;
    DescriptorPool CreateDescriptorPool(uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes, VkDescriptorPoolCreateFlags createFlags = 0) const;

public: // getter
    VkPhysicalDevice GetPhysicalDeviceHandle(void) const { return m_physicalDevice; }
    VkDevice GetHandle(void) const { return m_device; }
    uint32_t GetGraphicQueueFamilyIndex(void) const { return m_graphicsQueueFamilyIndex; }
    uint32_t GetPresentQueueFamilyIndex(void) const { return m_presentQueueFamilyIndex; }
    VkQueue GetGrahpicsQueue(void) const { return m_graphicsQueue; }
    VkQueue GetPresentQueue(void) const { return m_presentQueue; }

private:
    VkPhysicalDevice SelectPhysicalDevice(const VkInstance& instance);
    bool IsPhysicalDeviceSuitable(const VkPhysicalDevice& physicalDevice);
    void SelectQueueIndex(const VkSurfaceKHR& surface);
    void CreateLogicalDevice(const std::vector<const char*>& requiredExtensions);

private:
    VkPhysicalDevice m_physicalDevice { VK_NULL_HANDLE };
    VkDevice m_device { VK_NULL_HANDLE };
    uint32_t m_graphicsQueueFamilyIndex { UINT32_MAX };
    uint32_t m_presentQueueFamilyIndex { UINT32_MAX };
    VkQueue m_graphicsQueue { VK_NULL_HANDLE };
    VkQueue m_presentQueue { VK_NULL_HANDLE };
};
}
