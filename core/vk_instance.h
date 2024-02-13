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
    void Initialize(const VkApplicationInfo* pApplicationInfo, const std::vector<const char*>& requiredLayers, const std::vector<const char*>& requiredExtensions);
    void Destroy(void);

public: // getter
    VkInstance GetHandle(void) const { return m_handle; }

private:
    VkInstance m_handle { VK_NULL_HANDLE };
};
}
