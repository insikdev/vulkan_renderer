#pragma once

namespace VK {
class Instance;

class Device {
public:
    Device(const Instance* pInstance);
    ~Device();

private:
    VkPhysicalDevice SelectPhysicalDevice();
    void CreateLogicalDevice();

private:
    const Instance* p_instance;

private:
    VkPhysicalDevice m_physicalDevice { VK_NULL_HANDLE };
    VkDevice m_device { VK_NULL_HANDLE };
    VkQueue m_queue { VK_NULL_HANDLE };
};
}
