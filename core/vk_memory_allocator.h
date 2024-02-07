#pragma once

#include "common.h"

namespace VK {
class Instance;
class Device;
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
    void Initialize(const Instance* pInstance, const Device* pDevice);
    void Destroy(void);
    void CreateBuffer(Buffer* pBuffer, VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags = 0) const;
    void DestroyBuffer(Buffer* pBuffer) const;
    void* Map(Buffer* pBuffer) const;
    void Unmap(Buffer* pBuffer) const;
    void CreateImage(Image* pImage, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage) const;
    void DestroyImage(Image* pImage) const;

public: // getter
    VmaAllocator GetHandle(void) const { return m_handle; }

private:
    const Instance* p_instance { nullptr };
    const Device* p_device { nullptr };

private:
    VmaAllocator m_handle { VK_NULL_HANDLE };
};
}
