#pragma once

#include "common.h"

namespace VK {
class Queue {
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

private:
    VkQueue m_handle { VK_NULL_HANDLE };
    uint32_t m_queueFamilyIndex {};
    uint32_t m_queueIndex {};
};
}
