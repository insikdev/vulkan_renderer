#pragma once

namespace VK {
class CommandBuffer {
public:
    CommandBuffer() = default;
    CommandBuffer(const CommandBuffer&) = default;
    CommandBuffer(CommandBuffer&&) = default;
    ~CommandBuffer();
    CommandBuffer& operator=(const CommandBuffer&) = default;
    CommandBuffer& operator=(CommandBuffer&&) = default;

public:
    void Initialize(VkDevice device, VkCommandPool commandPool);
    void Free(void);
    void BeginRecording(VkCommandBufferUsageFlags usageFlags = 0);
    void EndRecording(void);
    void Reset(void);
    void Submit(VkQueue queue);
    void Submit(VkQueue queue, const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkSemaphore>& signalSemaphores, VkFence fence);

public: // getter
    VkCommandBuffer GetHandle(void) const { return m_handle; }

private:
    VkDevice m_device { VK_NULL_HANDLE };
    VkCommandPool m_commandPool { VK_NULL_HANDLE };
    VkCommandBuffer m_handle { VK_NULL_HANDLE };
};
}
