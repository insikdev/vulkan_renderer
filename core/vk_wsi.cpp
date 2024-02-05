#include "pch.h"
#include "vk_wsi.h"

#ifdef _WIN32
VK::WSI::WSI(const std::vector<const char*>& instanceLayers, const std::vector<const char*>& instanceExtensions, const std::vector<const char*>& deviceExtensions, HINSTANCE hinstance, HWND hwnd)
{
    m_instance.Initialize(instanceLayers, instanceExtensions);
    m_surface.Initialize(m_instance.GetHandle(), hinstance, hwnd);
    m_device.Initialize(m_instance.GetHandle(), m_surface.GetHandle(), deviceExtensions);
    m_swapchain.Initialize(&m_device, m_surface.GetHandle());
}
#endif

VK::WSI::~WSI()
{
    m_swapchain.Destroy();
    m_device.Destroy();
    m_surface.Destroy();
    m_instance.Destroy();
}
