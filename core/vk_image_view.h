#pragma once

#include "common.h"

namespace VK {
class Device;

class ImageView {
public:
    ImageView() = default;
    ~ImageView();
    ImageView(const ImageView&) = delete;
    ImageView(ImageView&&) noexcept;
    ImageView& operator=(const ImageView&) = delete;
    ImageView& operator=(ImageView&&) noexcept;

public:
    void Initialize(const Device* pDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void Destroy(void);

public: // getter
    VkImageView GetHandle(void) const { return m_handle; }

private:
    const Device* p_device { nullptr };

private:
    VkImageView m_handle { VK_NULL_HANDLE };
};
}
