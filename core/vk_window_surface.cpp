#include "pch.h"
#include "vk_window_surface.h"
#include "vk_instance.h"

VK::WinSurface::WinSurface(const Instance* pInstance, HINSTANCE hinstance, HWND hwnd)
    : p_instance { pInstance }
{
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo {
        .sType { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR },
        .pNext { nullptr },
        .flags {},
        .hinstance { hinstance },
        .hwnd { hwnd }
    };

    CHECK_VK(vkCreateWin32SurfaceKHR(p_instance->GetHandle(), &surfaceCreateInfo, nullptr, &m_surface), "Failed to create window surface.");
}

VK::WinSurface::~WinSurface()
{
    vkDestroySurfaceKHR(p_instance->GetHandle(), m_surface, nullptr);
}
