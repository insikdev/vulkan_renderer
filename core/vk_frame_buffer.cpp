#include "vk_frame_buffer.h"

VK::FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
    : m_handle { other.m_handle }
    , m_device { other.m_device }
{
    other.m_handle = VK_NULL_HANDLE;
}

VK::FrameBuffer& VK::FrameBuffer::operator=(FrameBuffer&& other) noexcept
{
    if (this != &other) {
        m_handle = other.m_handle;
        m_device = other.m_device;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

VkResult VK::FrameBuffer::Init(VkDevice device, VkRenderPass renderPass, const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
    }

    VkFramebufferCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO },
        .pNext {},
        .flags {},
        .renderPass { renderPass },
        .attachmentCount { static_cast<uint32_t>(attachments.size()) },
        .pAttachments { attachments.data() },
        .width { width },
        .height { height },
        .layers { 1 }
    };

    return vkCreateFramebuffer(m_device, &createInfo, nullptr, &m_handle);
}

void VK::FrameBuffer::Destroy()
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(m_device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
