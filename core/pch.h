#pragma once

#define WIN32_LEAN_AND_MEAN

#include <stdexcept>
#include <iostream>
#include <vector>

#include <vulkan\vulkan.h>

#define CHECK_VK(result, message)          \
    if (result != VK_SUCCESS) {            \
        std::cerr << message << std::endl; \
        throw std::runtime_error(message); \
    }