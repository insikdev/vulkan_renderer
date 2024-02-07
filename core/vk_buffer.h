#pragma once

#include "common.h"

namespace VK {
class CommandBuffer;

class Buffer {
public:
    Buffer() = default;
    ~Buffer();
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) noexcept;
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) noexcept;

public:
    void Initialize(const VmaAllocator& allocator, VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags = 0);
    void Destroy(void);

public: // method
    void CopyData(void* pSrc, VkDeviceSize size);
    void CopyToBuffer(const CommandBuffer& commandBuffer, const VkBuffer& dstBuffer, VkDeviceSize size);
    void CopyToImage(const CommandBuffer& commandBuffer, const VkImage& image, const VkExtent3D& extent3D);

public: // getter
    VkBuffer GetHandle(void) const { return m_handle; }

private:
    VmaAllocator m_allocator { VK_NULL_HANDLE };

private:
    VkBuffer m_handle { VK_NULL_HANDLE };
    VmaAllocation m_allocation { VK_NULL_HANDLE };
};
}
