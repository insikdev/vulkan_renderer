#pragma once

#include "common.h"

namespace VK {
class Instance {
public:
    Instance() = default;
    ~Instance() { Destroy(); }
    Instance(const Instance&) = delete;
    Instance(Instance&&) = delete;
    Instance& operator=(const Instance&) = delete;
    Instance& operator=(Instance&&) = delete;

public:
    VkResult Init(const VkApplicationInfo* pApplicationInfo, const std::vector<const char*>& enabledLayers, const std::vector<const char*>& enabledExtensions);
    void Destroy(void);

public:
    VkInstance GetHandle(void) const { return m_handle; }
    std::vector<VkPhysicalDevice> GetPhysicalDevices(void) const;

private:
    VkInstance m_handle { VK_NULL_HANDLE };
};
}
