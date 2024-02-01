#include "pch.h"
#include "vk_swapchain.h"
#include "vk_device.h"
#include "query.h"

VK::Swapchain::Swapchain(const Device* pDevice, VkSurfaceKHR surface)
    : p_device { pDevice }
    , m_surface { surface }
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p_device->GetPhysicalDeviceHandle(), surface, &m_capabilities);
    m_format = SelectFormat();
    m_presentMode = SelectPresentMode();

    CreateSwapchain();
    CreateImageViews();
}

VK::Swapchain::~Swapchain()
{
    for (auto imageView : m_imageViews) {
        vkDestroyImageView(p_device->GetHandle(), imageView, nullptr);
    }

    vkDestroySwapchainKHR(p_device->GetHandle(), m_swapchain, nullptr);
}

VkSurfaceFormatKHR VK::Swapchain::SelectFormat(void)
{
    std::vector<VkSurfaceFormatKHR> formats = Query::GetSurfaceFormats(p_device->GetPhysicalDeviceHandle(), m_surface);

    if (formats.empty()) {
        throw std::runtime_error("");
    }

    for (const auto& availableFormat : formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return formats[0];
}

VkPresentModeKHR VK::Swapchain::SelectPresentMode(void)
{
    std::vector<VkPresentModeKHR> presentModes = Query::GetPresentModes(p_device->GetPhysicalDeviceHandle(), m_surface);

    if (presentModes.empty()) {
        throw std::runtime_error("");
    }

    for (const auto& availablePresentMode : presentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

void VK::Swapchain::CreateSwapchain(void)
{
    uint32_t imageCount = m_capabilities.minImageCount + 1;

    if (m_capabilities.maxImageCount > 0 && imageCount > m_capabilities.maxImageCount) {
        imageCount = m_capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfoKHR {
        .sType { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR },
        .pNext { nullptr },
        .flags {},
        .surface { m_surface },
        .minImageCount { imageCount },
        .imageFormat { m_format.format },
        .imageColorSpace { m_format.colorSpace },
        .imageExtent { m_capabilities.currentExtent },
        .imageArrayLayers { 1 },
        .imageUsage { VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT },
        .imageSharingMode { VK_SHARING_MODE_EXCLUSIVE },
        .queueFamilyIndexCount {},
        .pQueueFamilyIndices { nullptr },
        .preTransform { m_capabilities.currentTransform },
        .compositeAlpha { VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR },
        .presentMode { m_presentMode },
        .clipped { VK_TRUE },
        .oldSwapchain { VK_NULL_HANDLE }
    };

    CHECK_VK(vkCreateSwapchainKHR(p_device->GetHandle(), &swapchainCreateInfoKHR, nullptr, &m_swapchain), "Failed to create swap chain.");

    vkGetSwapchainImagesKHR(p_device->GetHandle(), m_swapchain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(p_device->GetHandle(), m_swapchain, &imageCount, m_swapChainImages.data());
}

void VK::Swapchain::CreateImageViews(void)
{
    m_imageViews.resize(m_swapChainImages.size());

    for (size_t i = 0; i < m_swapChainImages.size(); i++) {
        VkComponentMapping components {
            .r { VK_COMPONENT_SWIZZLE_IDENTITY },
            .g { VK_COMPONENT_SWIZZLE_IDENTITY },
            .b { VK_COMPONENT_SWIZZLE_IDENTITY },
            .a { VK_COMPONENT_SWIZZLE_IDENTITY }
        };

        VkImageSubresourceRange subresourceRange {
            .aspectMask { VK_IMAGE_ASPECT_COLOR_BIT },
            .baseMipLevel { 0 },
            .levelCount { 1 },
            .baseArrayLayer { 0 },
            .layerCount { 1 }
        };

        VkImageViewCreateInfo imageViewCreateinfo {
            .sType { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO },
            .pNext {},
            .flags {},
            .image { m_swapChainImages[i] },
            .viewType { VK_IMAGE_VIEW_TYPE_2D },
            .format { m_format.format },
            .components { components },
            .subresourceRange { subresourceRange }
        };

        CHECK_VK(vkCreateImageView(p_device->GetHandle(), &imageViewCreateinfo, nullptr, &m_imageViews[i]), "Failed to create image view.");
    }
}
