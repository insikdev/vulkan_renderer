#include "pch.h"
#include "vk_surface.h"
#include "vk_instance.h"
#include "check_vk.h"

VK::Surface::~Surface()
{
    Destroy();
}

#ifdef _WIN32
void VK::Surface::Initialize(const Instance* pInstance, HINSTANCE hinstance, HWND hwnd)
{
    assert(m_handle == VK_NULL_HANDLE && pInstance != nullptr && hinstance != nullptr && hwnd != nullptr);

    {
        p_instance = pInstance;
    }

    VkWin32SurfaceCreateInfoKHR createInfo {
        .sType { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR },
        .pNext { nullptr },
        .flags {},
        .hinstance { hinstance },
        .hwnd { hwnd }
    };

    CHECK_VK(vkCreateWin32SurfaceKHR(p_instance->GetHandle(), &createInfo, nullptr, &m_handle), "Failed to create win32 surface.");
}
#endif

void VK::Surface::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(p_instance->GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
        p_instance = nullptr;
    }
}
