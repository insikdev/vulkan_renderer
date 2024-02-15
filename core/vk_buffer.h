#pragma once

#include "common.h"

namespace VK {
class CommandBuffer;

class Buffer {
public:
    Buffer() = default;
    ~Buffer() { Destroy(); }
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) noexcept;
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) noexcept;

public:
    VkResult Init(VmaAllocator allocator, VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags = 0);
    void Destroy(void);

public:
    VkBuffer GetHandle(void) const { return m_handle; }
    void CopyData(void* pSrc, VkDeviceSize size);
    void CopyToBuffer(const CommandBuffer& commandBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void CopyToImage(const CommandBuffer& commandBuffer, VkImage image, const VkExtent3D& extent3D);

private:
    VmaAllocator m_allocator { VK_NULL_HANDLE };
    VmaAllocation m_allocation { VK_NULL_HANDLE };
    VkBuffer m_handle { VK_NULL_HANDLE };
};
}
