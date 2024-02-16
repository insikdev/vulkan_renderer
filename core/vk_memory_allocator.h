#pragma once

#include "common.h"

namespace VK {
class Buffer;
class Image;
class CommandPool;
class CommandBuffer;

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
    Buffer CreateBufferUsingStaging(const CommandBuffer& commandBuffer, VkDeviceSize bufferSize, VkBufferUsageFlags usageFlags, void* pData) const;
    Image CreateImage(const VkExtent3D& extent3D, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags) const;
    Image CreateTexture2D(const std::string& filepath, const CommandPool& commandPool, VkQueue queue) const;

private:
    VmaAllocator m_handle { VK_NULL_HANDLE };
};
}
