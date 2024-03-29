#include "vk_image_view.h"

VK::ImageView::ImageView(ImageView&& other) noexcept
    : m_handle { other.m_handle }
    , m_device { other.m_device }
{
    other.m_handle = VK_NULL_HANDLE;
}

VK::ImageView& VK::ImageView::operator=(ImageView&& other) noexcept
{
    if (this != &other) {
        m_handle = other.m_handle;
        m_device = other.m_device;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

VkResult VK::ImageView::Init(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
    }

    VkComponentMapping components {
        .r { VK_COMPONENT_SWIZZLE_IDENTITY },
        .g { VK_COMPONENT_SWIZZLE_IDENTITY },
        .b { VK_COMPONENT_SWIZZLE_IDENTITY },
        .a { VK_COMPONENT_SWIZZLE_IDENTITY }
    };

    VkImageSubresourceRange subresourceRange {
        .aspectMask { aspectFlags },
        .baseMipLevel { 0 },
        .levelCount { 1 },
        .baseArrayLayer { 0 },
        .layerCount { 1 },
    };

    VkImageViewCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO },
        .pNext {},
        .flags {},
        .image { image },
        .viewType { VK_IMAGE_VIEW_TYPE_2D },
        .format { format },
        .components { components },
        .subresourceRange { subresourceRange },
    };

    return vkCreateImageView(m_device, &createInfo, nullptr, &m_handle);
}

void VK::ImageView::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyImageView(m_device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
