#define VMA_IMPLEMENTATION
#include "vk_memory_allocator.h"
#include "vk_buffer.h"
#include "vk_image.h"

void VK::MemoryAllocator::Initialize(const VkInstance& instance, const VkPhysicalDevice& physicalDevice, const VkDevice& device)
{
    assert(m_handle == VK_NULL_HANDLE);

    VmaAllocatorCreateInfo allocatorCreateInfo {
        .physicalDevice { physicalDevice },
        .device { device },
        .instance { instance }
    };

    CHECK_VK(vmaCreateAllocator(&allocatorCreateInfo, &m_handle), "Failed to create memory allocator.");
}

void VK::MemoryAllocator::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vmaDestroyAllocator(m_handle);
        m_handle = VK_NULL_HANDLE;
    }
}

VK::Buffer VK::MemoryAllocator::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VK::Buffer buffer;
    buffer.Initialize(m_handle, size, usage, allocationFlags);

    return buffer;
}

VK::Image VK::MemoryAllocator::CreateImage(const VkExtent3D& extent3D, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VK::Image image;
    image.Initialize(m_handle, extent3D, format, tiling, usage);

    return image;
}
