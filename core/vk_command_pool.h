#pragma once

#include "common.h"

namespace VK {
class CommandBuffer;

class CommandPool {
public:
    CommandPool() = default;
    ~CommandPool() { Destroy(); }
    CommandPool(const CommandPool&) = delete;
    CommandPool(CommandPool&&) noexcept;
    CommandPool& operator=(const CommandPool&) = delete;
    CommandPool& operator=(CommandPool&&) noexcept;

public:
    VkResult Init(VkDevice device, VkCommandPoolCreateFlags createFlags, uint32_t queueFamilyIndex);
    void Destroy(void);

public:
    VkCommandPool GetHandle(void) const { return m_handle; }
    CommandBuffer AllocateCommandBuffer(VkQueue queue) const;

private:
    VkDevice m_device { VK_NULL_HANDLE };
    VkCommandPool m_handle { VK_NULL_HANDLE };
};
}
