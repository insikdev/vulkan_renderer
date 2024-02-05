#pragma once

namespace VK {
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
    void Initialize(Device* pDevice, VkSurfaceKHR surface);
    void Destroy(void);
    void CreateFrameBuffers(VkRenderPass renderPass, VkImageView depthImageView);

public: // getter
    VkSwapchainKHR GetHandle(void) const { return m_handle; }
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
    VkSurfaceCapabilitiesKHR m_capabilities {};
    VkSurfaceFormatKHR m_format {};
    VkPresentModeKHR m_presentMode { VK_PRESENT_MODE_FIFO_KHR };
    VkSwapchainKHR m_handle { VK_NULL_HANDLE };
    std::vector<VkImage> m_images {};
    std::vector<VkImageView> m_imageViews {};
    std::vector<VkFramebuffer> m_framebuffers {};
};
}
