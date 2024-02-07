#pragma once

#include "common.h"

namespace VK {
class ImageView {
public:
    ImageView() = default;
    ~ImageView();
    ImageView(const ImageView&) = delete;
    ImageView(ImageView&&) noexcept;
    ImageView& operator=(const ImageView&) = delete;
    ImageView& operator=(ImageView&&) noexcept;

public:
    void Initialize(const VkDevice& device, const VkImage& image, VkFormat format, VkImageAspectFlags aspectFlags);
    void Destroy(void);

public: // getter
    VkImageView GetHandle(void) const { return m_handle; }

private:
    VkDevice m_device { VK_NULL_HANDLE };

private:
    VkImageView m_handle { VK_NULL_HANDLE };
};
}
