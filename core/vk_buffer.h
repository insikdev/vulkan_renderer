#pragma once

#include "common.h"

namespace VK {
class Device;

class Buffer {
    friend class MemoryAllocator;

public:
    Buffer() = default;
    ~Buffer();
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;

public:
    void Initialize(const MemoryAllocator* pAllocator, VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags = 0);
    void Destroy(void);
    void CopyData(void* pSrc, VkDeviceSize size);

public:
    static void CopyBufferToBuffer(const Device* pDevice, const Buffer* src, const Buffer* dst, VkDeviceSize size);

public: // getter
    VkBuffer GetHandle(void) const { return m_handle; }

private:
    const MemoryAllocator* p_allocator { nullptr };

private:
    VkBuffer m_handle { VK_NULL_HANDLE };
    VmaAllocation m_allocation { VK_NULL_HANDLE };
};
}
