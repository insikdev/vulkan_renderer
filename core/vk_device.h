#pragma once

namespace VK {
class Instance;

class Device {
public:
    Device(const Instance* pInstance, const std::vector<const char*>& extensions);
    ~Device();

public:
    VkDevice GetHandle(void) const { return m_device; }
    VkPhysicalDevice GetPhysicalDeviceHandle(void) const { return m_physicalDevice; }

private:
    VkPhysicalDevice SelectPhysicalDevice();
    void CreateLogicalDevice(const std::vector<const char*>& extensions);

private:
    const Instance* p_instance;

private:
    VkPhysicalDevice m_physicalDevice { VK_NULL_HANDLE };
    VkDevice m_device { VK_NULL_HANDLE };
    VkQueue m_queue { VK_NULL_HANDLE };
};
}
