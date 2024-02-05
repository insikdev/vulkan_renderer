#pragma once

namespace VK {
class CommandBuffer;

class CommandPool {
public:
    CommandPool() = default;
    CommandPool(const CommandPool&) = default;
    CommandPool(CommandPool&&) = default;
    ~CommandPool();
    CommandPool& operator=(const CommandPool&) = default;
    CommandPool& operator=(CommandPool&&) = default;

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
