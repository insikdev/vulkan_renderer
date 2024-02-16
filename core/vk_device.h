#pragma once

#include "common.h"

namespace VK {
class Device {
public:
    Device() = default;
    ~Device() { Destroy(); }
    Device(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) = delete;

public:
    VkResult Init(VkPhysicalDevice physicalDevice, const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos, const std::vector<const char*>& enabledExtensions, const VkPhysicalDeviceFeatures* pEnabledFeatures);
    void Destroy(void);

public:
    VkDevice GetHandle(void) const { return m_handle; }
    VkQueue GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex = 0) const;
    VkResult WaitIdle(void) const;

private:
    VkDevice m_handle { VK_NULL_HANDLE };
};
}
