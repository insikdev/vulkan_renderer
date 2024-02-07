#pragma once

#include "vk_image_view.h"

namespace VK {
class Surface;
class Device;

class Swapchain {
public:
    Swapchain() = default;
    ~Swapchain();
    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain(Swapchain&&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;

public:
    void Initialize(const Surface* pSurface, const Device* pDevice);
    void Destroy(void);
    void CreateFrameBuffers(VkRenderPass renderPass, VkImageView depthImageView);

public: // getter
    VkSwapchainKHR GetHandle(void) const { return m_handle; }
    VkExtent2D GetExtent(void) const { return m_capabilities.currentExtent; }
    VkSurfaceFormatKHR GetFormat(void) const { return m_format; }
    // std::vector<VkImageView> GetImageViews(void) const { return m_imageViews; }
    VkFramebuffer GetFrameBuffer(uint32_t index) const { return m_framebuffers[index]; }

private:
    VkSurfaceFormatKHR SelectFormat(void);
    VkPresentModeKHR SelectPresentMode(void);
    void CreateSwapchain(void);
    void CreateImageViews(void);

private:
    const Surface* p_surface { nullptr };
    const Device* p_device { nullptr };

private:
    VkSurfaceCapabilitiesKHR m_capabilities {};
    VkSurfaceFormatKHR m_format {};
    VkPresentModeKHR m_presentMode { VK_PRESENT_MODE_FIFO_KHR };
    VkSwapchainKHR m_handle { VK_NULL_HANDLE };
    std::vector<VkImage> m_images {};
    std::vector<ImageView> m_imageViews {};
    std::vector<VkFramebuffer> m_framebuffers {};
};
}
