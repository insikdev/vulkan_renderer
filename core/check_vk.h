#pragma once

inline void CHECK_VK(VkResult result, const std::string& message)
{
    if (result != VK_SUCCESS) {
        std::cerr << message << std::endl;
        throw std::runtime_error(message);
    }
}