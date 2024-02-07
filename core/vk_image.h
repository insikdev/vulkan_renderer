#pragma once

#include "common.h"

namespace VK {
class CommandBuffer;
class ImageView;

class Image {
public:
    Image() = default;
    ~Image();
    Image(const Image&) = delete;
    Image(Image&&) noexcept;
    Image& operator=(const Image&) = delete;
    Image& operator=(Image&&) noexcept;

public:
    void Initialize(const VmaAllocator& allocator, const VkExtent3D& extent3D, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
    void Destroy(void);

public: // method
    void TransitionLayout(const CommandBuffer& commandBuffer, const VkImageLayout& oldLayout, const VkImageLayout& newLayout);
    ImageView CreateView(const VkDevice& device, VkFormat format, VkImageAspectFlags aspectFlags) const;

public: // getter
    VkImage GetHandle(void) const { return m_handle; }

private:
    VmaAllocator m_allocator { VK_NULL_HANDLE };

private:
    VkImage m_handle { VK_NULL_HANDLE };
    VmaAllocation m_allocation { VK_NULL_HANDLE };
};
}
