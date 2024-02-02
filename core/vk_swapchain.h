#pragma once

namespace VK {
class Device;

class Swapchain {
public:
    Swapchain(const Device* pDevice, VkSurfaceKHR surface);
    ~Swapchain();

public: // getter
    VkSwapchainKHR GetHandle(void) const { return m_swapchain; }
    VkExtent2D GetExtent(void) const { return m_capabilities.currentExtent; }
    VkSurfaceFormatKHR GetFormat(void) const { return m_format; }
    std::vector<VkImageView> GetImageViews(void) const { return m_imageViews; }

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
