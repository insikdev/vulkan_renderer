#pragma once

#include "common.h"

namespace VK {
class Buffer;
class Image;

class MemoryAllocator {
public:
    MemoryAllocator() = default;
    ~MemoryAllocator() { Destroy(); }
    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator(MemoryAllocator&&) = delete;
    MemoryAllocator& operator=(const MemoryAllocator&) = delete;
    MemoryAllocator& operator=(MemoryAllocator&&) = delete;

public:
    VkResult Init(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device);
    void Destroy(void);

public:
    VmaAllocator GetHandle(void) const { return m_handle; }
    Buffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VmaAllocationCreateFlags allocationFlags = 0) const;
    Image CreateImage(const VkExtent3D& extent3D, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags) const;

private:
    VmaAllocator m_handle { VK_NULL_HANDLE };
};
}
