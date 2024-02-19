#define VMA_IMPLEMENTATION
#include "vk_memory_allocator.h"
#include "vk_buffer.h"
#include "vk_image.h"

VkResult VK::MemoryAllocator::Init(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device)
{
    assert(m_handle == VK_NULL_HANDLE);

    VmaAllocatorCreateInfo createInfo {
        .physicalDevice { physicalDevice },
        .device { device },
        .instance { instance }
    };

    return vmaCreateAllocator(&createInfo, &m_handle);
}

void VK::MemoryAllocator::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vmaDestroyAllocator(m_handle);
        m_handle = VK_NULL_HANDLE;
    }
}

VK::Buffer VK::MemoryAllocator::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VmaAllocationCreateFlags allocationFlags) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VK::Buffer buffer;
    buffer.Init(m_handle, size, usageFlags, allocationFlags);

    return buffer;
}

VK::Image VK::MemoryAllocator::CreateImage(const VkExtent3D& extent3D, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VK::Image image;
    image.Init(m_handle, extent3D, format, tiling, usageFlags);

    return image;
}
