#pragma once

namespace VK {
class Device;

class Swapchain {
public:
    Swapchain(Device* pDevice, VkSurfaceKHR surface);
    ~Swapchain();

public:
    void CreateFrameBuffers(VkRenderPass renderPass, VkImageView depthImageView);

public: // getter
    VkSwapchainKHR GetHandle(void) const { return m_swapchain; }
    VkExtent2D GetExtent(void) const { return m_capabilities.currentExtent; }
    VkSurfaceFormatKHR GetFormat(void) const { return m_format; }
    std::vector<VkImageView> GetImageViews(void) const { return m_imageViews; }
    VkFramebuffer GetFrameBuffer(uint32_t index) const { return m_framebuffers[index]; }

private:
    VkSurfaceFormatKHR SelectFormat(void);
    VkPresentModeKHR SelectPresentMode(void);
    void CreateSwapchain(void);
    void CreateImageViews(void);

private:
    Device* p_device { nullptr };
    VkSurfaceKHR m_surface { VK_NULL_HANDLE };

private:
    VkSurfaceCapabilitiesKHR m_capabilities;
    VkSurfaceFormatKHR m_format;
    VkPresentModeKHR m_presentMode;
    VkSwapchainKHR m_swapchain;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    std::vector<VkFramebuffer> m_framebuffers;
};
}
