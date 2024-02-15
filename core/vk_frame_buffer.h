#pragma once

#include "common.h"

namespace VK {
class FrameBuffer {
public:
    FrameBuffer() = default;
    ~FrameBuffer() { Destroy(); }
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer(FrameBuffer&&) noexcept;
    FrameBuffer& operator=(const FrameBuffer&) = delete;
    FrameBuffer& operator=(FrameBuffer&&) noexcept;

public:
    VkResult Init(VkDevice device, VkRenderPass renderPass, const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height);
    void Destroy();

public:
    VkFramebuffer GetHandle(void) const { return m_handle; }

private:
    VkDevice m_device { VK_NULL_HANDLE };
    VkFramebuffer m_handle { VK_NULL_HANDLE };
};
}
