#include "pch.h"
#include "vma.h"

// VK::VMA::VMA(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device)
//{
//    VmaAllocatorCreateInfo allocatorCreateInfo {
//        .physicalDevice { physicalDevice },
//        .device { device },
//        .instance { instance }
//    };
//
//    CHECK_VK(vmaCreateAllocator(&allocatorCreateInfo, &m_allocator), "Failed to create memory allocator");
//}
//
// VK::VMA::~VMA()
//{
//    vmaDestroyAllocator(m_allocator);
//}
//
// Buffer VK::VMA::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags)
//{
//    VkBufferCreateInfo bufferCreateInfo {
//        .sType { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO },
//        .pNext { nullptr },
//        .flags {},
//        .size { size },
//        .usage { usage },
//        .sharingMode {},
//        .queueFamilyIndexCount {},
//        .pQueueFamilyIndices {}
//    };
//
//    VmaAllocationCreateInfo allocationCreateInfo = {
//        .flags { allocationFlags },
//        .usage { VMA_MEMORY_USAGE_AUTO },
//        .requiredFlags {},
//        .preferredFlags {},
//        .memoryTypeBits {},
//        .pool {},
//        .pUserData {},
//        .priority {}
//    };
//
//    VK::Buffer buffer;
//
//    buffer.m_allocator = m_allocator;
//
//    CHECK_VK(vmaCreateBuffer(m_allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer.m_buffer, &buffer.m_allocation, &buffer.m_allocationInfo), "Failed to create buffer.");
//
//    return buffer;
//}
