#include "pch.h"
#include "vk_wsi.h"
#include "vk_instance.h"
#include "vk_surface.h"
#include "vk_device.h"
#include "vk_swapchain.h"

#ifdef _WIN32
VK::WSI::WSI(const std::vector<const char*>& instanceLayers, const std::vector<const char*>& instanceExtensions, const std::vector<const char*>& deviceExtensions, HINSTANCE hinstance, HWND hwnd)
{
    p_instance = new Instance { instanceLayers, instanceExtensions };
    p_surface = new Surface { p_instance->GetHandle(), hinstance, hwnd };
    p_device = new Device { p_instance->GetHandle(), p_surface->GetHandle(), deviceExtensions };
    p_swapchain = new Swapchain { p_device, p_surface->GetHandle() };
}
#endif

VK::WSI::~WSI()
{
    delete p_swapchain;
    delete p_device;
    delete p_surface;
    delete p_instance;
}
