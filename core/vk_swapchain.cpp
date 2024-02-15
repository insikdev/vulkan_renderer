#include "vk_swapchain.h"

VkResult VK::Swapchain::Init(VkDevice device, VkSurfaceKHR surface, uint32_t minImageCount, VkFormat imageFormat, VkColorSpaceKHR imageColorSpace, VkExtent2D imageExtent, VkSurfaceTransformFlagBitsKHR preTransform, VkPresentModeKHR presentMode)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
    }

    VkSwapchainCreateInfoKHR createInfo {
        .sType { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR },
        .pNext {},
        .flags {},
        .surface { surface },
        .minImageCount { minImageCount },
        .imageFormat { imageFormat },
        .imageColorSpace { imageColorSpace },
        .imageExtent { imageExtent },
        .imageArrayLayers { 1 },
        .imageUsage { VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT },
        .imageSharingMode { VK_SHARING_MODE_EXCLUSIVE },
        .queueFamilyIndexCount { /* only VK_SHARING_MODE_CONCURRENT */ },
        .pQueueFamilyIndices { /* only VK_SHARING_MODE_CONCURRENT */ },
        .preTransform { preTransform },
        .compositeAlpha { VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR },
        .presentMode { presentMode },
        .clipped { VK_TRUE },
        .oldSwapchain { VK_NULL_HANDLE }
    };

    return vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_handle);
}

void VK::Swapchain::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(m_device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

std::vector<VkImage> VK::Swapchain::GetPresentableImages(void) const
{
    uint32_t count {};
    vkGetSwapchainImagesKHR(m_device, m_handle, &count, nullptr);

    std::vector<VkImage> images(count);

    if (count != 0) {
        vkGetSwapchainImagesKHR(m_device, m_handle, &count, images.data());
    }

    return images;
}

VkResult VK::Swapchain::AcquireNextImageIndex(uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex) const
{
    return vkAcquireNextImageKHR(m_device, m_handle, timeout, semaphore, fence, pImageIndex);
}
