#pragma once

#include "vk_resource.h"
#include "vk_command_pool.h"
#include <vector>

namespace VK {
class CommandPool;

class Device {
public:
    Device(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& requiredExtensions);
    ~Device();

public: // buffer
    Buffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags = 0);
    void DestroyBuffer(Buffer buffer);
    void CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
    void CopyDataToDevice(VmaAllocation allocation, void* pSrc, VkDeviceSize size);

public: // image
    Image CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
    void DestroyImage(Image image);
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

public: // getter
    VkPhysicalDevice GetPhysicalDeviceHandle(void) const { return m_physicalDevice; }
    VkDevice GetHandle(void) const { return m_device; }
    uint32_t GetGraphicQueueFamilyIndex(void) const { return m_graphicsQueueFamilyIndex; }
    uint32_t GetPresentQueueFamilyIndex(void) const { return m_presentQueueFamilyIndex; }
    VkQueue GetGrahpicsQueue(void) const { return m_graphicsQueue; }
    VkQueue GetPresentQueue(void) const { return m_presentQueue; }
    const VK::CommandPool& GetCommandPool(void) const { return m_commandPool; }

private:
    VkPhysicalDevice SelectPhysicalDevice(void);
    void SelectQueueIndex(VkSurfaceKHR surface);
    void CreateLogicalDevice(const std::vector<const char*>& requiredExtensions);
    void CreateMemoryAllocator(void);

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

    VK::CommandPool m_commandPool;
};
}
