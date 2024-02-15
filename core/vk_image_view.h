#pragma once

#include "common.h"

namespace VK {
class ImageView {
public:
    ImageView() = default;
    ~ImageView() { Destroy(); }
    ImageView(const ImageView&) = delete;
    ImageView(ImageView&&) noexcept;
    ImageView& operator=(const ImageView&) = delete;
    ImageView& operator=(ImageView&&) noexcept;

public:
    VkResult Init(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void Destroy(void);

public:
    VkImageView GetHandle(void) const { return m_handle; }

private:
    VkDevice m_device { VK_NULL_HANDLE };
    VkImageView m_handle { VK_NULL_HANDLE };
};
}
