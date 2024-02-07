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
    void Initialize(const VkDevice& device, const VkCommandPool& commandPool, const VkQueue& queueToSubmit);
    void Destroy(void);

public: // method
    void BeginRecording(VkCommandBufferUsageFlags usageFlags = 0) const;
    void EndRecording(void) const;
    void Reset(void) const;
    void Submit(void) const;
    void Submit(const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkSemaphore>& signalSemaphores, VkFence fence) const;

public: // getter
    VkCommandBuffer GetHandle(void) const { return m_handle; }

private:
    VkDevice m_device { VK_NULL_HANDLE };
    VkCommandPool m_commandPool { VK_NULL_HANDLE };
    VkQueue m_queueToSubmit { VK_NULL_HANDLE };

private:
    VkCommandBuffer m_handle { VK_NULL_HANDLE };
};
}
