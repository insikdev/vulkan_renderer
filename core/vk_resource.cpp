#define VMA_IMPLEMENTATION
#include "vk_resource.h"
#include "check_vk.h"

VK::MemoryAllocator::~MemoryAllocator()
{
    Destroy();
}

void VK::MemoryAllocator::Initialize(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device)
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
