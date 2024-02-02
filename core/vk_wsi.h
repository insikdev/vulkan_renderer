#pragma once

#include "vk_instance.h"
#include "vk_device.h"
#include "vk_swapchain.h"

namespace VK {
class WSI {
public:
    WSI() = delete;
    ~WSI();

public:
    static std::unique_ptr<WSI> CreateWin32(
        const std::vector<const char*>& instanceLayers,
        const std::vector<const char*>& instanceExtensions,
        const std::vector<const char*>& deviceExtensions,
        HINSTANCE hinstance, HWND hwnd);

public: // getter
    VkDevice GetDevice(void) const { return p_device->GetHandle(); }
    VkExtent2D GetExtent(void) const { return p_swapchain->GetExtent(); }
    VkFormat GetFormat(void) const { return p_swapchain->GetFormat().format; }
    std::vector<VkImageView> GetImageViews(void) const { return p_swapchain->GetImageViews(); }
    VkSwapchainKHR GetSwapchain(void) const { return p_swapchain->GetHandle(); }
    VkQueue GetGrahpicsQueue(void) const { return p_device->GetGrahpicsQueue(); }
    VkQueue GetPresentQueue(void) const { return p_device->GetPresentQueue(); }

private:
    WSI(Instance* pInstance, Device* pDevice, Swapchain* pSwapchain, VkSurfaceKHR surface);

private:
    Instance* p_instance;
    Device* p_device;
    Swapchain* p_swapchain;
    VkSurfaceKHR m_surface;
};
}
