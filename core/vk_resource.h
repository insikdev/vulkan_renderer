#pragma once

#include <vk_mem_alloc.h>

namespace VK {
class Resource {
public:
    Resource(VmaAllocator m_allocator);
    virtual ~Resource() = 0;

protected:
    VmaAllocator m_allocator { VK_NULL_HANDLE };
    VmaAllocation m_allocation {};
    VmaAllocationInfo m_allocationInfo {};
};
}
