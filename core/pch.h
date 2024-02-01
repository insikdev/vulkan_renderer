#pragma once

#include <stdexcept>
#include <iostream>
#include <vector>
#include <memory>
#include <set>
#include <limits>
#include <cstdint>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan\vulkan.h>

#define CHECK_VK(result, message)          \
    if (result != VK_SUCCESS) {            \
        std::cerr << message << std::endl; \
        throw std::runtime_error(message); \
    }