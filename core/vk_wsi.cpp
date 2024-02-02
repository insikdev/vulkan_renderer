#include "pch.h"
#include "vk_wsi.h"

VK::WSI::WSI(Instance* pInstance, Device* pDevice, Swapchain* pSwapchain, VkSurfaceKHR surface)
    : p_instance { pInstance }
    , p_device { pDevice }
    , p_swapchain { pSwapchain }
    , m_surface { surface }
{
}

VK::WSI::~WSI()
{
    delete p_swapchain;
    delete p_device;
    vkDestroySurfaceKHR(p_instance->GetHandle(), m_surface, nullptr);
    delete p_instance;
}

std::unique_ptr<VK::WSI> VK::WSI::CreateWin32(
    const std::vector<const char*>& instanceLayers,
    const std::vector<const char*>& instanceExtensions,
    const std::vector<const char*>& deviceExtensions,
    HINSTANCE hinstance, HWND hwnd)
{
    Instance* pInstance = new Instance { instanceLayers, instanceExtensions };

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo {
        .sType { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR },
        .pNext { nullptr },
        .flags {},
        .hinstance { hinstance },
        .hwnd { hwnd }
    };

    VkSurfaceKHR surface { VK_NULL_HANDLE };

    CHECK_VK(vkCreateWin32SurfaceKHR(pInstance->GetHandle(), &surfaceCreateInfo, nullptr, &surface), "Failed to create win32 surface.");

    Device* pDevice = new Device { pInstance, surface, deviceExtensions };
    Swapchain* pSwapchain = new Swapchain { pDevice, surface };

    return std::unique_ptr<WSI>(new WSI { pInstance, pDevice, pSwapchain, surface });
}
