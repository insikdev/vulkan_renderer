#include "pch.h"
#include "vk_image.h"
#include "vk_device.h"
#include "vk_command_pool.h"
#include "vk_command_buffer.h"
#include "vk_buffer.h"
#include "vk_image_view.h"

VK::Image::~Image()
{
    Destroy();
}

void VK::Image::Initialize(const MemoryAllocator* pAllocator, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage)
{
    assert(m_handle == VK_NULL_HANDLE && pAllocator != nullptr);

    {
        p_allocator = pAllocator;
    }

    p_allocator->CreateImage(this, width, height, format, tiling, usage);
}

void VK::Image::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        p_allocator->DestroyImage(this);
        m_handle = VK_NULL_HANDLE;
        p_allocator = nullptr;
    }
}

VK::ImageView VK::Image::CreateView(const Device* pDevice, VkFormat format, VkImageAspectFlags aspectFlags) const
{
    ImageView view;
    view.Initialize(pDevice, m_handle, format, aspectFlags);

    return view;
}

void VK::Image::CopyBufferToImage(const Device* pDevice, Buffer* pBuffer, Image* pImage, uint32_t width, uint32_t height)
{
    VK::CommandBuffer commandBuffer;
    commandBuffer.Initialize(pDevice, pDevice->GetCommandPool());
    commandBuffer.BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkImageSubresourceLayers imageSubresource {
        .aspectMask { VK_IMAGE_ASPECT_COLOR_BIT },
        .mipLevel { 0 },
        .baseArrayLayer { 0 },
        .layerCount { 1 }
    };

    VkBufferImageCopy region {
        .bufferOffset { 0 },
        .bufferRowLength { 0 },
        .bufferImageHeight { 0 },
        .imageSubresource { imageSubresource },
        .imageOffset { 0, 0, 0 },
        .imageExtent { width, height, 1 }
    };

    vkCmdCopyBufferToImage(commandBuffer.GetHandle(), pBuffer->GetHandle(), pImage->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    commandBuffer.EndRecording();
    commandBuffer.Submit(pDevice->GetGrahpicsQueue());
}
