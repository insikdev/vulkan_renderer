#pragma once

#include "common.h"

namespace VK {
class Device;
class CommandBuffer;

class CommandPool {
public:
    CommandPool() = default;
    ~CommandPool();
    CommandPool(const CommandPool&) = delete;
    CommandPool(CommandPool&&) = delete;
    CommandPool& operator=(const CommandPool&) = delete;
    CommandPool& operator=(CommandPool&&) = delete;

public:
    void Initialize(const Device* pDevice, VkCommandPoolCreateFlags createFlags = 0);
    void Destroy(void);
    CommandBuffer AllocateCommandBuffer(void) const;

public: // getter
    VkCommandPool GetHandle(void) const { return m_handle; }

private:
    const Device* p_device { nullptr };

private:
    VkCommandPool m_handle { VK_NULL_HANDLE };
};
}
