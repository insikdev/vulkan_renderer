#pragma once

#define WIN32_LEAN_AND_MEAN

#include <exception>

#include <vulkan\vulkan.h>

#define CHECK_VK(result)        \
    if (result != VK_SUCCESS) { \
        throw std::exception(); \
    }