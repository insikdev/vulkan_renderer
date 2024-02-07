#define VMA_IMPLEMENTATION
#include "vk_memory_allocator.h"
#include "vk_instance.h"
#include "vk_device.h"
#include "vk_buffer.h"
#include "vk_image.h"

VK::MemoryAllocator::~MemoryAllocator()
{
    Destroy();
}

void VK::MemoryAllocator::Initialize(const Instance* pInstance, const Device* pDevice)
{
    assert(m_handle == VK_NULL_HANDLE && pInstance != nullptr && pDevice != nullptr);

    {
        p_instance = pInstance;
        p_device = pDevice;
    }

    VmaAllocatorCreateInfo allocatorCreateInfo {
        .physicalDevice { p_device->GetPhysicalDeviceHandle() },
        .device { p_device->GetHandle() },
        .instance { p_instance->GetHandle() }
    };

    CHECK_VK(vmaCreateAllocator(&allocatorCreateInfo, &m_handle), "Failed to create memory allocator.");
}

void VK::MemoryAllocator::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vmaDestroyAllocator(m_handle);
        m_handle = VK_NULL_HANDLE;
        p_instance = nullptr;
        p_device = nullptr;
    }
}

void VK::MemoryAllocator::CreateBuffer(Buffer* pBuffer, VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags) const
{
    assert(m_handle != VK_NULL_HANDLE && pBuffer != nullptr);

    VkBufferCreateInfo bufferCreateInfo {
        .sType { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .size { size },
        .usage { usage },
        .sharingMode {},
        .queueFamilyIndexCount {},
        .pQueueFamilyIndices {}
    };

    VmaAllocationCreateInfo allocationCreateInfo = {
        .flags { allocationFlags },
        .usage { VMA_MEMORY_USAGE_AUTO },
        .requiredFlags {},
        .preferredFlags {},
        .memoryTypeBits {},
        .pool {},
        .pUserData {},
        .priority {}
    };

    CHECK_VK(vmaCreateBuffer(m_handle, &bufferCreateInfo, &allocationCreateInfo, &pBuffer->m_handle, &pBuffer->m_allocation, nullptr), "Failed to create buffer.");
}

void VK::MemoryAllocator::DestroyBuffer(Buffer* pBuffer) const
{
    assert(m_handle != VK_NULL_HANDLE && pBuffer != nullptr);

    vmaDestroyBuffer(m_handle, pBuffer->m_handle, pBuffer->m_allocation);
}

void* VK::MemoryAllocator::Map(Buffer* pBuffer) const
{
    assert(m_handle != VK_NULL_HANDLE && pBuffer != nullptr);

    void* pData;
    vmaMapMemory(m_handle, pBuffer->m_allocation, &pData);
    return pData;
}

void VK::MemoryAllocator::Unmap(Buffer* pBuffer) const
{
    assert(m_handle != VK_NULL_HANDLE && pBuffer != nullptr);

    vmaUnmapMemory(m_handle, pBuffer->m_allocation);
}

void VK::MemoryAllocator::CreateImage(Image* pImage, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage) const
{
    assert(m_handle != VK_NULL_HANDLE && pImage != nullptr);

    VkImageCreateInfo imageInfo {
        .sType { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .imageType { VK_IMAGE_TYPE_2D },
        .format { format },
        .extent { width, height, 1 },
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

    VmaAllocationCreateInfo allocationCreateInfo = {
        .flags {},
        .usage { VMA_MEMORY_USAGE_AUTO },
        .requiredFlags {},
        .preferredFlags {},
        .memoryTypeBits {},
        .pool {},
        .pUserData {},
        .priority {}
    };

    CHECK_VK(vmaCreateImage(m_handle, &imageInfo, &allocationCreateInfo, &pImage->m_handle, &pImage->m_allocation, nullptr), "Failed to create image.");
}

void VK::MemoryAllocator::DestroyImage(Image* pImage) const
{
    assert(m_handle != VK_NULL_HANDLE && pImage != nullptr);

    vmaDestroyImage(m_handle, pImage->m_handle, pImage->m_allocation);
}
