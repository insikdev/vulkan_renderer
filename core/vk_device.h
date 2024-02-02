#pragma once

namespace VK {
class Instance;

class Device {
public:
    Device(const Instance* pInstance, VkSurfaceKHR surface, const std::vector<const char*>& requiredExtensions);
    ~Device();

public:
    VkDevice GetHandle(void) const { return m_device; }
    VkPhysicalDevice GetPhysicalDeviceHandle(void) const { return m_physicalDevice; }
    uint32_t GetGraphicQueueFamilyIndex(void) const { return m_graphicsQueueFamilyIndex; }
    uint32_t GetPresentQueueFamilyIndex(void) const { return m_presentQueueFamilyIndex; }
    VkQueue GetGrahpicsQueue(void) const { return m_graphicsQueue; }
    VkQueue GetPresentQueue(void) const { return m_presentQueue; }

private:
    VkPhysicalDevice SelectPhysicalDevice();
    void SelectQueueIndex(VkSurfaceKHR surface);
    void CreateLogicalDevice(const std::vector<const char*>& requiredExtensions);

private:
    const Instance* p_instance;

private:
    VkPhysicalDevice m_physicalDevice { VK_NULL_HANDLE };
    VkDevice m_device { VK_NULL_HANDLE };
    VkQueue m_graphicsQueue { VK_NULL_HANDLE };
    VkQueue m_presentQueue { VK_NULL_HANDLE };
    uint32_t m_graphicsQueueFamilyIndex { UINT32_MAX };
    uint32_t m_presentQueueFamilyIndex { UINT32_MAX };
};
}
