#pragma once

namespace VK {
class Device;

class Swapchain {
public:
    Swapchain(const Device* pDevice, VkSurfaceKHR surface);
    ~Swapchain();

private:
    VkSurfaceFormatKHR SelectFormat(void);
    VkPresentModeKHR SelectPresentMode(void);
    void CreateSwapchain(void);
    void CreateImageViews(void);

private:
    const Device* p_device;
    VkSurfaceKHR m_surface;
    VkSurfaceCapabilitiesKHR m_capabilities;
    VkSurfaceFormatKHR m_format;
    VkPresentModeKHR m_presentMode;
    VkSwapchainKHR m_swapchain;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
};
}
