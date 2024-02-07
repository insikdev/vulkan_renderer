#pragma once

#include "common.h"

namespace VK {
class Fence {
public:
    Fence() = default;
    ~Fence() { Destroy(); }
    Fence(const Fence&) = delete;
    Fence(Fence&&) = delete;
    Fence& operator=(const Fence&) = delete;
    Fence& operator=(Fence&&) = delete;

public:
    void Initialize(const VkDevice& device, VkFenceCreateFlags createFlags);
    void Destroy(void);

public: // getter
    VkFence GetHandle(void) const { return m_handle; }

private:
    VkDevice m_device { VK_NULL_HANDLE };

private:
    VkFence m_handle { VK_NULL_HANDLE };
};
}
