#pragma once

#include "common.h"

namespace VK {
class Swapchain {
public:
    Swapchain() = default;
    ~Swapchain() { Destroy(); }
    Swapchain(const Swapchain&) = delete;
    Swapchain(Swapchain&&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;

public:
    VkResult Init(VkDevice device, VkSurfaceKHR surface, uint32_t minImageCount, VkFormat imageFormat, VkColorSpaceKHR imageColorSpace, VkExtent2D imageExtent, VkSurfaceTransformFlagBitsKHR preTransform, VkPresentModeKHR presentMode);
    void Destroy(void);

public:
    VkSwapchainKHR GetHandle(void) const { return m_handle; }
    std::vector<VkImage> GetPresentableImages(void) const;
    VkResult AcquireNextImageIndex(uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex) const;

private:
    VkDevice m_device { VK_NULL_HANDLE };
    VkSwapchainKHR m_handle { VK_NULL_HANDLE };
};
}
