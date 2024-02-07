#pragma once

#include "common.h"

namespace VK {
class CommandBuffer;

class CommandPool {
public:
    CommandPool() = default;
    ~CommandPool() { Destroy(); }
    CommandPool(const CommandPool&) = delete;
    CommandPool(CommandPool&&) = delete;
    CommandPool& operator=(const CommandPool&) = delete;
    CommandPool& operator=(CommandPool&&) = delete;

public:
    void Initialize(const VkDevice& device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags = 0);
    void Destroy(void);

public: // method
    CommandBuffer AllocateCommandBuffer(const VkQueue& queueToSubmit) const;

public: // getter
    VkCommandPool GetHandle(void) const { return m_handle; }

private:
    VkDevice m_device { VK_NULL_HANDLE };

private:
    VkCommandPool m_handle { VK_NULL_HANDLE };
};
}
