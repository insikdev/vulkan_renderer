#pragma once

#include "common.h"
#include "vk_command_pool.h"
#include "vk_memory_allocator.h"

namespace VK {
class Instance;
class Surface;

class Device {
public:
    Device() = default;
    ~Device();
    Device(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) = delete;

public:
    void Initialize(const Instance* pInstance, const Surface* pSurface, const std::vector<const char*>& requiredExtensions);
    void Destroy(void);

public: // getter
    VkPhysicalDevice GetPhysicalDeviceHandle(void) const { return m_physicalDevice; }
    VkDevice GetHandle(void) const { return m_device; }
    uint32_t GetGraphicQueueFamilyIndex(void) const { return m_graphicsQueueFamilyIndex; }
    uint32_t GetPresentQueueFamilyIndex(void) const { return m_presentQueueFamilyIndex; }
    VkQueue GetGrahpicsQueue(void) const { return m_graphicsQueue; }
    VkQueue GetPresentQueue(void) const { return m_presentQueue; }
    const CommandPool* GetCommandPool(void) const { return &m_commandPool; }
    const MemoryAllocator* GetMemoryAllocator(void) const { return &m_allocator; }

private:
    VkPhysicalDevice SelectPhysicalDevice(void);
    void SelectQueueIndex(VkSurfaceKHR surface);
    void CreateLogicalDevice(const std::vector<const char*>& requiredExtensions);

private:
    const Instance* p_instance { nullptr };
    const Surface* p_surface { nullptr };

private:
    VkPhysicalDevice m_physicalDevice { VK_NULL_HANDLE };
    VkDevice m_device { VK_NULL_HANDLE };
    uint32_t m_graphicsQueueFamilyIndex { UINT32_MAX };
    uint32_t m_presentQueueFamilyIndex { UINT32_MAX };
    VkQueue m_graphicsQueue { VK_NULL_HANDLE };
    VkQueue m_presentQueue { VK_NULL_HANDLE };

    CommandPool m_commandPool {};
    MemoryAllocator m_allocator {};
};
}
