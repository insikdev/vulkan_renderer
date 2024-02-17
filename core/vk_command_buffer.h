#pragma once

#include "common.h"

namespace VK {
class CommandBuffer {
public:
    CommandBuffer() = default;
    ~CommandBuffer() { Destroy(); }
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer(CommandBuffer&&) noexcept;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&&) noexcept;

public:
    VkResult Init(VkDevice device, VkCommandPool commandPool);
    void Destroy(void);

public:
    VkCommandBuffer GetHandle(void) const { return m_handle; }
    VkResult BeginRecording(VkCommandBufferUsageFlags usageFlags = 0) const;
    VkResult EndRecording(void) const;
    VkResult Reset(void) const;

private:
    VkDevice m_device { VK_NULL_HANDLE };
    VkCommandPool m_commandPool { VK_NULL_HANDLE };
    VkCommandBuffer m_handle { VK_NULL_HANDLE };
};
}
