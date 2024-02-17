#pragma once

#include "common.h"

namespace VK {
class Queue {
public:
    Queue() = default;
    ~Queue() = default;
    Queue(const Queue&) = delete;
    Queue(Queue&&) noexcept;
    Queue& operator=(const Queue&) = delete;
    Queue& operator=(Queue&&) noexcept;

public:
    void Init(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex);

public:
    VkQueue GetHandle(void) const { return m_handle; }
    uint32_t GetFamilyIndex(void) const { return m_queueFamilyIndex; }
    uint32_t GetIndex(void) const { return m_queueIndex; }
    VkResult WaitIdle(void) const;
    VkResult Submit(const std::vector<VkSubmitInfo>& submitInfos, VkFence fence) const;
    VkResult Submit(const std::vector<VkCommandBuffer>& commandBuffers, VkFence fence = VK_NULL_HANDLE) const;
    VkResult Present(const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkSwapchainKHR>& swapchains, const uint32_t* pImageIndices) const;

private:
    VkQueue m_handle { VK_NULL_HANDLE };
    uint32_t m_queueFamilyIndex {};
    uint32_t m_queueIndex {};
};
}
