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