#pragma once

#include "common.h"

namespace VK {
class Buffer;
class Image;

class MemoryAllocator {
public:
    MemoryAllocator() = default;
    ~MemoryAllocator();
    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator(MemoryAllocator&&) = delete;
    MemoryAllocator& operator=(const MemoryAllocator&) = delete;
    MemoryAllocator& operator=(MemoryAllocator&&) = delete;

public:
    void Initialize(const VkInstance& instance, const VkPhysicalDevice& physicalDevice, const VkDevice& device);
    void Destroy(void);

public: // method
    Buffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags = 0) const;
    Image CreateImage(const VkExtent3D& extent3D, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage) const;

public: // getter
    VmaAllocator GetHandle(void) const { return m_handle; }

private:
    VmaAllocator m_handle { VK_NULL_HANDLE };
};
}
