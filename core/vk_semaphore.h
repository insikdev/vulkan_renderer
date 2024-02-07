#pragma once

#include "common.h"

namespace VK {
class Semaphore {
public:
    Semaphore() = default;
    ~Semaphore() { Destroy(); }
    Semaphore(const Semaphore&) = delete;
    Semaphore(Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;

public:
    void Initialize(const VkDevice& device);
    void Destroy(void);

public: // getter
    VkSemaphore GetHandle(void) const { return m_handle; }

private:
    VkDevice m_device { VK_NULL_HANDLE };

private:
    VkSemaphore m_handle { VK_NULL_HANDLE };
};
}
