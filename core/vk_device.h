#pragma once

#include "vk_resource.h"

namespace VK {
class Device {
public:
    Device(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& requiredExtensions);
    ~Device();

public:
    Buffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags = 0);
    void DestroyBuffer(Buffer buffer);
    void CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
    void CopyDataToDevice(VmaAllocation allocation, void* pSrc, VkDeviceSize size);
    Image CreateImage(VkExtent3D extent);
    void DestroyImage(Image image);
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    VkImageView CreateImageView(VkImage image, VkFormat format);

    VkCommandBuffer BeginSingleTimeCommands(void);
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

public: // getter
    VkPhysicalDevice GetPhysicalDeviceHandle(void) const { return m_physicalDevice; }
    VkDevice GetHandle(void) const { return m_device; }
    uint32_t GetGraphicQueueFamilyIndex(void) const { return m_graphicsQueueFamilyIndex; }
    uint32_t GetPresentQueueFamilyIndex(void) const { return m_presentQueueFamilyIndex; }
    VkQueue GetGrahpicsQueue(void) const { return m_graphicsQueue; }
    VkQueue GetPresentQueue(void) const { return m_presentQueue; }
    VkCommandPool GetCommandPool(void) const { return m_commandPool; }

private:
    VkPhysicalDevice SelectPhysicalDevice(void);
    void SelectQueueIndex(VkSurfaceKHR surface);
    void CreateLogicalDevice(const std::vector<const char*>& requiredExtensions);
    void CreateMemoryAllocator(void);
    void CreateCommandPool(void);

private:
    VkInstance m_instance;

private:
    VkPhysicalDevice m_physicalDevice { VK_NULL_HANDLE };
    VkDevice m_device { VK_NULL_HANDLE };
    uint32_t m_graphicsQueueFamilyIndex { UINT32_MAX };
    uint32_t m_presentQueueFamilyIndex { UINT32_MAX };
    VkQueue m_graphicsQueue { VK_NULL_HANDLE };
    VkQueue m_presentQueue { VK_NULL_HANDLE };
    VmaAllocator m_allocator { VK_NULL_HANDLE };
    VkCommandPool m_commandPool { VK_NULL_HANDLE };
};
}
