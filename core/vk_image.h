#pragma once

#include "vk_memory_allocator.h"

namespace VK {
class Device;
class Buffer;
class ImageView;

class Image {
    friend class MemoryAllocator;

public:
    Image() = default;
    ~Image();
    Image(const Image&) = delete;
    Image(Image&&) = delete;
    Image& operator=(const Image&) = delete;
    Image& operator=(Image&&) = delete;

public:
    void Initialize(const MemoryAllocator* pAllocator, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
    void Destroy(void);
    ImageView CreateView(const Device* pDevice, VkFormat format, VkImageAspectFlags aspectFlags) const;

public:
    static void CopyBufferToImage(const Device* pDevice, Buffer* pBuffer, Image* pImage, uint32_t width, uint32_t height);

public: // getter
    VkImage GetHandle(void) const { return m_handle; }

private:
    const MemoryAllocator* p_allocator { nullptr };

private:
    VkImage m_handle { VK_NULL_HANDLE };
    VmaAllocation m_allocation { VK_NULL_HANDLE };
};
}
