#include "vk_surface.h"

VkResult VK::Surface::Init(VkInstance instance, HINSTANCE hinstance, HWND hwnd)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_instance = instance;
    }

    VkWin32SurfaceCreateInfoKHR createInfo {
        .sType { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR },
        .pNext {},
        .flags {},
        .hinstance { hinstance },
        .hwnd { hwnd }
    };

    return vkCreateWin32SurfaceKHR(m_instance, &createInfo, nullptr, &m_handle);
}

void VK::Surface::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
