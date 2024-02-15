#pragma once

#include "common.h"

namespace VK {
class CommandBuffer;
class ImageView;

class Image {
public:
    Image() = default;
    ~Image() { Destroy(); }
    Image(const Image&) = delete;
    Image(Image&&) noexcept;
    Image& operator=(const Image&) = delete;
    Image& operator=(Image&&) noexcept;

public:
    VkResult Init(VmaAllocator allocator, const VkExtent3D& extent3D, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
    void Destroy(void);

public:
    VkImage GetHandle(void) const { return m_handle; }
    void TransitionLayout(const CommandBuffer& commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);
    ImageView CreateView(VkDevice device, VkFormat format, VkImageAspectFlags aspectFlags) const;

private:
    VmaAllocator m_allocator { VK_NULL_HANDLE };
    VmaAllocation m_allocation { VK_NULL_HANDLE };
    VkImage m_handle { VK_NULL_HANDLE };
};
}
