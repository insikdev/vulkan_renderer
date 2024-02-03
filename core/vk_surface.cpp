#include "pch.h"
#include "vk_surface.h"

#ifdef _WIN32
VK::Surface::Surface(VkInstance instance, HINSTANCE hinstance, HWND hwnd)
    : m_instance { instance }
{
    VkWin32SurfaceCreateInfoKHR createInfo {
        .sType { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR },
        .pNext { nullptr },
        .flags {},
        .hinstance { hinstance },
        .hwnd { hwnd }
    };

    CHECK_VK(vkCreateWin32SurfaceKHR(m_instance, &createInfo, nullptr, &m_surface), "Failed to create win32 surface.");
}
#endif

VK::Surface::~Surface()
{
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
}
