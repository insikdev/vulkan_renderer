#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <vector>
#include <cstdint>
#include <cassert>
#include <memory>
#include <set>
#include <stdexcept>

inline void CHECK_VK(VkResult result, const char* message)
{
    if (result != VK_SUCCESS) {
        throw std::runtime_error(message);
    }
}