#include "pch.h"
#include "vk_buffer.h"
#include <vk_mem_alloc.h>

VK::Buffer::Buffer(VmaAllocator allocator)
    : Resource { allocator }
{
}

VK::Buffer::~Buffer()
{
    vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
}

void VK::Buffer::CopyDataToDevice(void* pSrc, VkDeviceSize size)
{
    void* mappedData;
    vmaMapMemory(m_allocator, m_allocation, &mappedData);
    memcpy(mappedData, pSrc, size);
    vmaUnmapMemory(m_allocator, m_allocation);
}