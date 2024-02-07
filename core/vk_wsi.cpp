#include "vk_wsi.h"

void VK::WSI::Initialize(
    const std::vector<const char*>& instanceLayers,
    const std::vector<const char*>& instanceExtensions,
    const std::vector<const char*>& deviceExtensions,
    const HINSTANCE& hinstance,
    const HWND& hwnd)
{
    assert(m_initialized == false);

    m_instance.Initialize(instanceLayers, instanceExtensions);
    m_surface.Initialize(m_instance.GetHandle(), hinstance, hwnd);
    m_device.Initialize(m_instance.GetHandle(), m_surface.GetHandle(), deviceExtensions);
    m_swapchain.Initialize(m_surface.GetHandle(), m_device.GetPhysicalDeviceHandle(), m_device.GetHandle(), m_device.GetGraphicQueueFamilyIndex(), m_device.GetPresentQueueFamilyIndex());
    m_commandPool.Initialize(m_device.GetHandle(), m_device.GetGraphicQueueFamilyIndex());
    m_memoryAllocator.Initialize(m_instance.GetHandle(), m_device.GetPhysicalDeviceHandle(), m_device.GetHandle());
}

void VK::WSI::Destroy(void)
{
    if (m_initialized == true) {
        m_memoryAllocator.Destroy();
        m_commandPool.Destroy();
        m_swapchain.Destroy();
        m_device.Destroy();
        m_surface.Destroy();
        m_instance.Destroy();

        m_initialized = false;
    }
}
