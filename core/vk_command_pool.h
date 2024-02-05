#pragma once

namespace VK {
class CommandBuffer;

class CommandPool {
public:
    CommandPool() = default;
    ~CommandPool();
    CommandPool(const CommandPool&) = delete;
    CommandPool& operator=(const CommandPool&) = delete;
    CommandPool(CommandPool&&) noexcept;
    CommandPool& operator=(CommandPool&&) noexcept;

public:
    void Initialize(VkDevice device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags = 0);
    void Destroy(void);
    CommandBuffer AllocateCommandBuffer(void) const;
    std::unique_ptr<CommandBuffer> AllocateCommandBufferUPTR(void) const;

public: // getter
    VkCommandPool GetHandle(void) const { return m_handle; }

private:
    VkDevice m_device { VK_NULL_HANDLE };
    VkCommandPool m_handle { VK_NULL_HANDLE };
};
}
