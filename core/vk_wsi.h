#pragma once

namespace VK {
class Instance;
class Device;
class Swapchain;

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

private:
    WSI(Instance* pInstance, Device* pDevice, Swapchain* pSwapchain, VkSurfaceKHR surface);

private:
    Instance* p_instance;
    Device* p_device;
    Swapchain* p_swapchain;
    VkSurfaceKHR m_surface;
};
}
