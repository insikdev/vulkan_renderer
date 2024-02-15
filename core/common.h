#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include <vector>
#include <cstdint>
#include <cassert>
#include <memory>
#include <set>
#include <iostream>

#define CHECK_VK(result, message)                                        \
    if (result != VK_SUCCESS) {                                          \
        std::cerr << "Error Code: " << result << ". " << message << '\n' \
                  << __FILE__ << " : " << __LINE__ << std::endl;         \
        std::exit(EXIT_FAILURE);                                         \
    }
