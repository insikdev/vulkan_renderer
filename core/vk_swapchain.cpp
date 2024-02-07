#include "vk_swapchain.h"
#include "query.h"

VK::Swapchain::~Swapchain()
{
    Destroy();
}

void VK::Swapchain::Initialize(const VkSurfaceKHR& surface, const VkPhysicalDevice& physicalDevice, const VkDevice& device, uint32_t graphicQueueFamilyIndex, uint32_t presentQueueFamilyIndex)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
    }

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &m_capabilities);
    m_format = SelectFormat(physicalDevice, surface);
    m_presentMode = SelectPresentMode(physicalDevice, surface);
    CreateSwapchain(surface, graphicQueueFamilyIndex, presentQueueFamilyIndex);
    CreateImageViews();
}

void VK::Swapchain::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        for (auto framebuffer : m_framebuffers) {
            vkDestroyFramebuffer(m_device, framebuffer, nullptr);
        }

        for (auto& imageView : m_imageViews) {
            imageView.Destroy();
        }

        vkDestroySwapchainKHR(m_device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

void VK::Swapchain::CreateFrameBuffers(const VkRenderPass& renderPass, const VkImageView& depthImageView)
{
    assert(m_handle != VK_NULL_HANDLE);

    m_framebuffers.resize(m_imageViews.size());

    for (size_t i = 0; i < m_imageViews.size(); i++) {
        std::vector<VkImageView> attachments { m_imageViews[i].GetHandle(), depthImageView };

        VkFramebufferCreateInfo framebufferInfo {
            .sType { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO },
            .pNext { nullptr },
            .flags {},
            .renderPass { renderPass },
            .attachmentCount { static_cast<uint32_t>(attachments.size()) },
            .pAttachments { attachments.data() },
            .width { m_capabilities.currentExtent.width },
            .height { m_capabilities.currentExtent.height },
            .layers { 1 },
        };

        CHECK_VK(vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffers[i]), "Failed to create frame buffer.");
    }
}

VkSurfaceFormatKHR VK::Swapchain::SelectFormat(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
{
    const auto& availableFormats = Query::GetSurfaceFormats(physicalDevice, surface);

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

VkPresentModeKHR VK::Swapchain::SelectPresentMode(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
{
    const auto& availablePresentModes = Query::GetPresentModes(physicalDevice, surface);

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

void VK::Swapchain::CreateSwapchain(const VkSurfaceKHR& surface, uint32_t graphicQueueFamilyIndex, uint32_t presentQueueFamilyIndex)
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
        .surface { surface },
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

    if (graphicQueueFamilyIndex != presentQueueFamilyIndex) {
        uint32_t queueFamilyIndices[] = { graphicQueueFamilyIndex, presentQueueFamilyIndex };

        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    CHECK_VK(vkCreateSwapchainKHR(m_device, &swapchainCreateInfo, nullptr, &m_handle), "Failed to create swap chain.");

    vkGetSwapchainImagesKHR(m_device, m_handle, &imageCount, nullptr);
    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device, m_handle, &imageCount, m_images.data());
}

void VK::Swapchain::CreateImageViews(void)
{
    m_imageViews.resize(m_images.size());

    for (size_t i = 0; i < m_images.size(); i++) {
        m_imageViews[i].Initialize(m_device, m_images[i], m_format.format, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}
