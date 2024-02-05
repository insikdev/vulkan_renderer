#pragma once

#include <stdexcept>

inline void CHECK_VK(VkResult result, const char* message)
{
    if (result != VK_SUCCESS) {
        throw std::runtime_error(message);
    }
}