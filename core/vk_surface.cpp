#include "vk_surface.h"

VK::Surface::~Surface()
{
    Destroy();
}

void VK::Surface::Initialize(const VkInstance& instance, const HINSTANCE& hinstance, const HWND& hwnd)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_instance = instance;
    }

    VkWin32SurfaceCreateInfoKHR createInfo {
        .sType { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR },
        .pNext { nullptr },
        .flags {},
        .hinstance { hinstance },
        .hwnd { hwnd }
    };

    CHECK_VK(vkCreateWin32SurfaceKHR(m_instance, &createInfo, nullptr, &m_handle), "Failed to create win32 surface.");
}

void VK::Surface::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
