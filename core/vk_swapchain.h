#pragma once

#include "common.h"
#include "vk_image_view.h"

namespace VK {
class Swapchain {
public:
    Swapchain() = default;
    ~Swapchain() { Destroy(); }
    Swapchain(const Swapchain&) = delete;
    Swapchain(Swapchain&&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;

public:
    void Initialize(const VkSurfaceKHR& surface, const VkPhysicalDevice& physicalDevice, const VkDevice& device, uint32_t graphicQueueFamilyIndex, uint32_t presentQueueFamilyIndex);
    void Destroy(void);

public: // method
    void CreateFrameBuffers(const VkRenderPass& renderPass, const VkImageView& depthImageView);

public: // getter
    VkSwapchainKHR GetHandle(void) const { return m_handle; }
    VkExtent2D GetExtent(void) const { return m_capabilities.currentExtent; }
    VkSurfaceFormatKHR GetFormat(void) const { return m_format; }
    VkFramebuffer GetFrameBuffer(uint32_t index) const { return m_framebuffers[index]; }

private:
    VkSurfaceFormatKHR SelectFormat(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
    VkPresentModeKHR SelectPresentMode(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
    void CreateSwapchain(const VkSurfaceKHR& surface, uint32_t graphicQueueFamilyIndex, uint32_t presentQueueFamilyIndex);
    void CreateImageViews(void);

private:
    VkDevice m_device { VK_NULL_HANDLE };

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
