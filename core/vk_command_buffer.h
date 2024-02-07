#pragma once

namespace VK {
class Device;
class CommandPool;

class CommandBuffer {
public:
    CommandBuffer() = default;
    ~CommandBuffer();
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer(CommandBuffer&&) noexcept;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&&) noexcept;

public:
    void Initialize(const Device* pDevice, const CommandPool* pCommandPool);
    void Destroy(void);
    void BeginRecording(VkCommandBufferUsageFlags usageFlags = 0);
    void EndRecording(void);
    void Reset(void);
    void Submit(VkQueue queue);
    void Submit(VkQueue queue, const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkSemaphore>& signalSemaphores, VkFence fence);

public: // getter
    VkCommandBuffer GetHandle(void) const { return m_handle; }

private:
    const Device* p_device { nullptr };
    const CommandPool* p_commandPool { nullptr };

private:
    VkCommandBuffer m_handle { VK_NULL_HANDLE };
};
}
