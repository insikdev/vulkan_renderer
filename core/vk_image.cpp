#include "vk_image.h"
#include "vk_command_buffer.h"
#include "vk_image_view.h"

VK::Image::Image(Image&& other) noexcept
    : m_handle { other.m_handle }
    , m_allocation { other.m_allocation }
    , m_allocator { other.m_allocator }
{
    other.m_handle = VK_NULL_HANDLE;
}

VK::Image& VK::Image::operator=(Image&& other) noexcept
{
    if (this != &other) {
        m_handle = other.m_handle;
        m_allocation = other.m_allocation;
        m_allocator = other.m_allocator;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

VkResult VK::Image::Init(VmaAllocator allocator, const VkExtent3D& extent3D, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_allocator = allocator;
    }

    VkImageCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO },
        .pNext {},
        .flags {},
        .imageType { VK_IMAGE_TYPE_2D },
        .format { format },
        .extent { extent3D },
        .mipLevels { 1 },
        .arrayLayers { 1 },
        .samples { VK_SAMPLE_COUNT_1_BIT },
        .tiling { tiling },
        .usage { usage },
        .sharingMode { VK_SHARING_MODE_EXCLUSIVE },
        .queueFamilyIndexCount {},
        .pQueueFamilyIndices {},
        .initialLayout { VK_IMAGE_LAYOUT_UNDEFINED },
    };

    VmaAllocationCreateInfo allocationInfo {
        .flags {},
        .usage { VMA_MEMORY_USAGE_AUTO },
        .requiredFlags {},
        .preferredFlags {},
        .memoryTypeBits {},
        .pool {},
        .pUserData {},
        .priority {}
    };

    return vmaCreateImage(m_allocator, &createInfo, &allocationInfo, &m_handle, &m_allocation, nullptr);
}

void VK::Image::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vmaDestroyImage(m_allocator, m_handle, m_allocation);
        m_handle = VK_NULL_HANDLE;
    }
}

void VK::Image::TransitionLayout(const CommandBuffer& commandBuffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask) const
{
    VkImageSubresourceRange subresourceRange {
        .aspectMask { VK_IMAGE_ASPECT_COLOR_BIT },
        .baseMipLevel { 0 },
        .levelCount { VK_REMAINING_MIP_LEVELS },
        .baseArrayLayer { 0 },
        .layerCount { VK_REMAINING_ARRAY_LAYERS },
    };

    VkImageMemoryBarrier barrier {
        .sType { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER },
        .pNext { nullptr },
        .srcAccessMask { srcAccessMask },
        .dstAccessMask { dstAccessMask },
        .oldLayout { oldLayout },
        .newLayout { newLayout },
        .srcQueueFamilyIndex { VK_QUEUE_FAMILY_IGNORED },
        .dstQueueFamilyIndex { VK_QUEUE_FAMILY_IGNORED },
        .image { m_handle },
        .subresourceRange { subresourceRange }
    };

    vkCmdPipelineBarrier(commandBuffer.GetHandle(), srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

VK::ImageView VK::Image::CreateView(VkDevice device, VkFormat format, VkImageAspectFlags aspectFlags) const
{
    ImageView view;
    view.Init(device, m_handle, format, aspectFlags);

    return view;
}
