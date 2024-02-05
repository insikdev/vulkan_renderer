#pragma once

#include <vk_mem_alloc.h>

namespace VK {
struct Buffer {
    VkBuffer handle;
    VmaAllocation allocation;
};

struct Image {
    VkImage handle;
    VmaAllocation allocation;
};
}

namespace VK {
class MemoryAllocator {
public:
    MemoryAllocator() = default;
    ~MemoryAllocator();
    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator& operator=(const MemoryAllocator&) = delete;
    MemoryAllocator(MemoryAllocator&&) = delete;
    MemoryAllocator& operator=(MemoryAllocator&&) = delete;

public:
    void Initialize(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device);
    void Destroy(void);

public: // getter
    VmaAllocator GetHandle(void) const { return m_handle; }

private:
    VmaAllocator m_handle;
};
}

namespace VK {
class Resource {
public:
    Resource() = default;
    virtual ~Resource() = 0;

protected:
    VmaAllocation m_allocation;
};

}