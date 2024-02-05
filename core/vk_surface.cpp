#include "pch.h"
#include "vk_surface.h"
#include "check_vk.h"

VK::Surface::~Surface()
{
    Destroy();
}

#ifdef _WIN32
void VK::Surface::Initialize(VkInstance instance, HINSTANCE hinstance, HWND hwnd)
{
    assert(m_handle == VK_NULL_HANDLE && instance != VK_NULL_HANDLE && hinstance != nullptr && hwnd != nullptr);

    m_instance = instance;

    VkWin32SurfaceCreateInfoKHR createInfo {
        .sType { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR },
        .pNext { nullptr },
        .flags {},
        .hinstance { hinstance },
        .hwnd { hwnd }
    };

    CHECK_VK(vkCreateWin32SurfaceKHR(m_instance, &createInfo, nullptr, &m_handle), "Failed to create win32 surface.");
}
#endif

void VK::Surface::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
