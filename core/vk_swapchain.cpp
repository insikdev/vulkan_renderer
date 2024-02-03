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
    for (auto framebuffer : m_framebuffers) {
        vkDestroyFramebuffer(p_device->GetHandle(), framebuffer, nullptr);
    }

    for (auto imageView : m_imageViews) {
        vkDestroyImageView(p_device->GetHandle(), imageView, nullptr);
    }

    vkDestroySwapchainKHR(p_device->GetHandle(), m_swapchain, nullptr);
}

void VK::Swapchain::CreateFrameBuffers(VkRenderPass renderPass)
{
    m_framebuffers.resize(m_imageViews.size());

    for (size_t i = 0; i < m_imageViews.size(); i++) {
        VkImageView attachments[] = { m_imageViews[i] };

        VkFramebufferCreateInfo framebufferInfo {
            .sType { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO },
            .pNext { nullptr },
            .flags {},
            .renderPass { renderPass },
            .attachmentCount { 1 },
            .pAttachments { attachments },
            .width { m_capabilities.currentExtent.width },
            .height { m_capabilities.currentExtent.height },
            .layers { 1 },
        };

        CHECK_VK(vkCreateFramebuffer(p_device->GetHandle(), &framebufferInfo, nullptr, &m_framebuffers[i]), "Failed to create frame buffer.");
    }
}

VkSurfaceFormatKHR VK::Swapchain::SelectFormat(void)
{
    const auto& availableFormats = Query::GetSurfaceFormats(p_device->GetPhysicalDeviceHandle(), m_surface);

    if (availableFormats.empty()) {
        throw std::runtime_error("Failed to create swap chain.");
    }

    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VK::Swapchain::SelectPresentMode(void)
{
    const auto& availablePresentModes = Query::GetPresentModes(p_device->GetPhysicalDeviceHandle(), m_surface);

    if (availablePresentModes.empty()) {
        throw std::runtime_error("Failed to create swap chain.");
    }

    for (const auto& availablePresentMode : availablePresentModes) {
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

    if (m_capabilities.currentExtent.width == UINT32_MAX) {
        throw std::runtime_error("Failed to create swap chain.");
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo {
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

    if (p_device->GetGraphicQueueFamilyIndex() != p_device->GetPresentQueueFamilyIndex()) {
        uint32_t queueFamilyIndices[] = { p_device->GetGraphicQueueFamilyIndex(), p_device->GetPresentQueueFamilyIndex() };

        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    CHECK_VK(vkCreateSwapchainKHR(p_device->GetHandle(), &swapchainCreateInfo, nullptr, &m_swapchain), "Failed to create swap chain.");

    vkGetSwapchainImagesKHR(p_device->GetHandle(), m_swapchain, &imageCount, nullptr);
    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(p_device->GetHandle(), m_swapchain, &imageCount, m_images.data());
}

void VK::Swapchain::CreateImageViews(void)
{
    m_imageViews.resize(m_images.size());

    for (size_t i = 0; i < m_images.size(); i++) {
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
            .image { m_images[i] },
            .viewType { VK_IMAGE_VIEW_TYPE_2D },
            .format { m_format.format },
            .components { components },
            .subresourceRange { subresourceRange }
        };

        CHECK_VK(vkCreateImageView(p_device->GetHandle(), &imageViewCreateinfo, nullptr, &m_imageViews[i]), "Failed to create image view.");
    }
}
