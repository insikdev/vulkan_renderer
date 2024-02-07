#include "vk_swapchain.h"
#include "vk_surface.h"
#include "vk_device.h"
#include "query.h"

VK::Swapchain::~Swapchain()
{
    Destroy();
}

void VK::Swapchain::Initialize(const Surface* pSurface, const Device* pDevice)
{
    assert(m_handle == VK_NULL_HANDLE && pSurface != nullptr && pDevice != nullptr);

    {
        p_surface = pSurface;
        p_device = pDevice;
    }

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p_device->GetPhysicalDeviceHandle(), p_surface->GetHandle(), &m_capabilities);
    m_format = SelectFormat();
    m_presentMode = SelectPresentMode();
    CreateSwapchain();
    CreateImageViews();
}

void VK::Swapchain::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        for (auto framebuffer : m_framebuffers) {
            vkDestroyFramebuffer(p_device->GetHandle(), framebuffer, nullptr);
        }

        for (auto& imageView : m_imageViews) {
            imageView.Destroy();
        }

        vkDestroySwapchainKHR(p_device->GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
        p_surface = nullptr;
        p_device = nullptr;
    }
}

void VK::Swapchain::CreateFrameBuffers(VkRenderPass renderPass, VkImageView depthImageView)
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

        CHECK_VK(vkCreateFramebuffer(p_device->GetHandle(), &framebufferInfo, nullptr, &m_framebuffers[i]), "Failed to create frame buffer.");
    }
}

VkSurfaceFormatKHR VK::Swapchain::SelectFormat(void)
{
    const auto& availableFormats = Query::GetSurfaceFormats(p_device->GetPhysicalDeviceHandle(), p_surface->GetHandle());

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
    const auto& availablePresentModes = Query::GetPresentModes(p_device->GetPhysicalDeviceHandle(), p_surface->GetHandle());

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
        .surface { p_surface->GetHandle() },
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

    CHECK_VK(vkCreateSwapchainKHR(p_device->GetHandle(), &swapchainCreateInfo, nullptr, &m_handle), "Failed to create swap chain.");

    vkGetSwapchainImagesKHR(p_device->GetHandle(), m_handle, &imageCount, nullptr);
    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(p_device->GetHandle(), m_handle, &imageCount, m_images.data());
}

void VK::Swapchain::CreateImageViews(void)
{
    m_imageViews.resize(m_images.size());

    for (size_t i = 0; i < m_images.size(); i++) {
        m_imageViews[i].Initialize(p_device, m_images[i], m_format.format, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}
