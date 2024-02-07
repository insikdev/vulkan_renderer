#include "pch.h"
#include "vk_image_view.h"
#include "vk_device.h"
#include "check_vk.h"

VK::ImageView::~ImageView()
{
    Destroy();
}

VK::ImageView::ImageView(ImageView&& other) noexcept
    : m_handle { other.m_handle }
    , p_device { other.p_device }
{
    other.m_handle = VK_NULL_HANDLE;
    other.p_device = nullptr;
}

VK::ImageView& VK::ImageView::operator=(ImageView&& other) noexcept
{
    if (this != &other) {
        m_handle = other.m_handle;
        p_device = other.p_device;

        other.m_handle = VK_NULL_HANDLE;
        other.p_device = nullptr;
    }

    return *this;
}

void VK::ImageView::Initialize(const Device* pDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    assert(m_handle == VK_NULL_HANDLE && pDevice != nullptr);

    {
        p_device = pDevice;
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

    CHECK_VK(vkCreateImageView(p_device->GetHandle(), &createInfo, nullptr, &m_handle), "Failed to create image view.");
}

void VK::ImageView::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyImageView(p_device->GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
        p_device = nullptr;
    }
}
