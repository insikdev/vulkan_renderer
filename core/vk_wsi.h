#pragma once

#include "vk_instance.h"
#include "vk_surface.h"
#include "vk_device.h"
#include "vk_swapchain.h"

namespace VK {
class WSI {
#ifdef _WIN32
public:
    WSI(const std::vector<const char*>& instanceLayers,
        const std::vector<const char*>& instanceExtensions,
        const std::vector<const char*>& deviceExtensions,
        HINSTANCE hinstance,
        HWND hwnd);
#endif

public:
    ~WSI();
    WSI(const WSI&) = delete;
    WSI(WSI&&) = delete;
    WSI& operator=(const WSI&) = delete;
    WSI& operator=(WSI&&) = delete;

public: // getter
    Device* GetDevice(void) { return &m_device; }
    Swapchain* GetSwapchain(void) { return &m_swapchain; }

private:
    Instance m_instance {};
    Surface m_surface {};
    Device m_device {};
    Swapchain m_swapchain {};
};
}
