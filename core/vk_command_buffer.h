#pragma once

#include "common.h"

namespace VK {
class CommandBuffer {
public:
    CommandBuffer() = default;
    ~CommandBuffer() = default;
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer(CommandBuffer&&) noexcept;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&&) noexcept;

public:
    VkResult Init(VkDevice device, VkCommandPool commandPool);
    void Free(void);

public:
    VkCommandBuffer GetHandle(void) const { return m_handle; }
    VkResult Begin(VkCommandBufferUsageFlags usageFlags = 0) const;
    VkResult End(void) const;
    VkResult Reset(VkCommandBufferResetFlags resetFlags = 0) const;
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

private:
    VkDevice m_device { VK_NULL_HANDLE };
    VkCommandPool m_commandPool { VK_NULL_HANDLE };
    VkCommandBuffer m_handle { VK_NULL_HANDLE };
};
}
