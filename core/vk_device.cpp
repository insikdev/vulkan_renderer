#include "pch.h"
#include "vk_device.h"
#include "query.h"
#include "utils.h"

VK::Device::Device(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& requiredExtensions)
    : m_instance { instance }
{
    m_physicalDevice = SelectPhysicalDevice();

    if (Utils::CheckExtensionSupport(requiredExtensions, Query::GetDeviceExtensions(m_physicalDevice)) == false) {
        throw std::runtime_error("Required extensions are not supported.");
    }

    SelectQueueIndex(surface);
    CreateLogicalDevice(requiredExtensions);
    CreateMemoryAllocator();
    CreateCommandPool();
}

VK::Device::~Device()
{
    vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    vmaDestroyAllocator(m_allocator);
    vkDestroyDevice(m_device, nullptr);
}

VK::Buffer VK::Device::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags)
{
    VkBufferCreateInfo bufferCreateInfo {
        .sType { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .size { size },
        .usage { usage },
        .sharingMode {},
        .queueFamilyIndexCount {},
        .pQueueFamilyIndices {}
    };

    VmaAllocationCreateInfo allocationCreateInfo = {
        .flags { allocationFlags },
        .usage { VMA_MEMORY_USAGE_AUTO },
        .requiredFlags {},
        .preferredFlags {},
        .memoryTypeBits {},
        .pool {},
        .pUserData {},
        .priority {}
    };

    Buffer buffer {};
    CHECK_VK(vmaCreateBuffer(m_allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer.handle, &buffer.allocation, nullptr), "Failed to create buffer.");

    return buffer;
}

void VK::Device::DestroyBuffer(Buffer buffer)
{
    vmaDestroyBuffer(m_allocator, buffer.handle, buffer.allocation);
}

void VK::Device::CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

    VkBufferCopy copyRegion {
        .srcOffset {},
        .dstOffset {},
        .size { size }
    };

    vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

    EndSingleTimeCommands(commandBuffer);
}

void VK::Device::CopyDataToDevice(VmaAllocation allocation, void* pSrc, VkDeviceSize size)
{
    void* mappedData;
    vmaMapMemory(m_allocator, allocation, &mappedData);
    memcpy(mappedData, pSrc, size);
    vmaUnmapMemory(m_allocator, allocation);
}

VK::Image VK::Device::CreateImage(VkExtent3D extent)
{
    VkImageCreateInfo imageInfo {
        .sType { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .imageType { VK_IMAGE_TYPE_2D },
        .format { VK_FORMAT_R8G8B8A8_SRGB },
        .extent { extent },
        .mipLevels { 1 },
        .arrayLayers { 1 },
        .samples { VK_SAMPLE_COUNT_1_BIT },
        .tiling { VK_IMAGE_TILING_OPTIMAL },
        .usage { VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT },
        .sharingMode { VK_SHARING_MODE_EXCLUSIVE },
        .queueFamilyIndexCount {},
        .pQueueFamilyIndices {},
        .initialLayout { VK_IMAGE_LAYOUT_UNDEFINED },
    };

    VmaAllocationCreateInfo allocationCreateInfo = {
        .flags {},
        .usage { VMA_MEMORY_USAGE_AUTO },
        .requiredFlags {},
        .preferredFlags {},
        .memoryTypeBits {},
        .pool {},
        .pUserData {},
        .priority {}
    };

    VK::Image image {};
    CHECK_VK(vmaCreateImage(m_allocator, &imageInfo, &allocationCreateInfo, &image.handle, &image.allocation, nullptr), "Failed to create image.");

    return image;
}

void VK::Device::DestroyImage(Image image)
{
    vmaDestroyImage(m_allocator, image.handle, image.allocation);
}

void VK::Device::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

    VkImageSubresourceLayers imageSubresource {
        .aspectMask { VK_IMAGE_ASPECT_COLOR_BIT },
        .mipLevel { 0 },
        .baseArrayLayer { 0 },
        .layerCount { 1 }
    };

    VkBufferImageCopy region {
        .bufferOffset { 0 },
        .bufferRowLength { 0 },
        .bufferImageHeight { 0 },
        .imageSubresource { imageSubresource },
        .imageOffset { 0, 0, 0 },
        .imageExtent { width, height, 1 }
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    EndSingleTimeCommands(commandBuffer);
}

VkImageView VK::Device::CreateImageView(VkImage image, VkFormat format)
{
    VkComponentMapping components {
        .r { VK_COMPONENT_SWIZZLE_IDENTITY },
        .g { VK_COMPONENT_SWIZZLE_IDENTITY },
        .b { VK_COMPONENT_SWIZZLE_IDENTITY },
        .a { VK_COMPONENT_SWIZZLE_IDENTITY }
    };

    VkImageSubresourceRange subresourceRange {
        .aspectMask { VK_IMAGE_ASPECT_COLOR_BIT },
        .baseMipLevel { 0 },
        .levelCount { 1 },
        .baseArrayLayer { 0 },
        .layerCount { 1 },
    };

    VkImageViewCreateInfo viewInfo {
        .sType { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO },
        .pNext {},
        .flags {},
        .image { image },
        .viewType { VK_IMAGE_VIEW_TYPE_2D },
        .format { format },
        .components { components },
        .subresourceRange { subresourceRange },
    };

    VkImageView imageView;
    CHECK_VK(vkCreateImageView(m_device, &viewInfo, nullptr, &imageView), "Failed to create image view.");

    return imageView;
}

VkCommandBuffer VK::Device::BeginSingleTimeCommands(void)
{
    VkCommandBufferAllocateInfo allocInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },
        .pNext { nullptr },
        .commandPool { m_commandPool },
        .level { VK_COMMAND_BUFFER_LEVEL_PRIMARY },
        .commandBufferCount { 1 }
    };

    VkCommandBuffer commandBuffer;
    CHECK_VK(vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer), "");

    VkCommandBufferBeginInfo beginInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
        .pNext { nullptr },
        .flags {},
        .pInheritanceInfo {}
    };

    CHECK_VK(vkBeginCommandBuffer(commandBuffer, &beginInfo), "");

    return commandBuffer;
}

void VK::Device::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    CHECK_VK(vkEndCommandBuffer(commandBuffer), "");

    VkSubmitInfo submitInfo {
        .sType { VK_STRUCTURE_TYPE_SUBMIT_INFO },
        .pNext { nullptr },
        .waitSemaphoreCount {},
        .pWaitSemaphores {},
        .pWaitDstStageMask {},
        .commandBufferCount { 1 },
        .pCommandBuffers { &commandBuffer },
        .signalSemaphoreCount {},
        .pSignalSemaphores {}
    };

    CHECK_VK(vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE), "");
    CHECK_VK(vkQueueWaitIdle(m_graphicsQueue), "");

    vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
}

VkPhysicalDevice VK::Device::SelectPhysicalDevice()
{
    const auto& devices = Query::GetPhysicalDevices(m_instance);

    if (devices.empty()) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support.");
    }

    if (devices.size() == 1) {
        return devices[0];
    }

    for (const auto& device : devices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.samplerAnisotropy) {
            return device;
        }
    }

    throw std::runtime_error("Failed to select physical device.");
}

void VK::Device::SelectQueueIndex(VkSurfaceKHR surface)
{
    const auto& queueFamilies = Query::GetQueueFamilies(m_physicalDevice);

    for (size_t i = 0; i < queueFamilies.size(); i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            m_graphicsQueueFamilyIndex = static_cast<uint32_t>(i);
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, static_cast<uint32_t>(i), surface, &presentSupport);

        if (presentSupport) {
            m_presentQueueFamilyIndex = static_cast<uint32_t>(i);
        }

        if (m_graphicsQueueFamilyIndex != UINT32_MAX && m_presentQueueFamilyIndex != UINT32_MAX) {
            break;
        }
    }

    if (m_graphicsQueueFamilyIndex == UINT32_MAX || m_presentQueueFamilyIndex == UINT32_MAX) {
        throw std::runtime_error("Failed to find queue family.");
    }
}

void VK::Device::CreateLogicalDevice(const std::vector<const char*>& requiredExtensions)
{
    float queuePriority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { m_graphicsQueueFamilyIndex, m_presentQueueFamilyIndex };

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo deviceQueueCreateInfo {
            .sType { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO },
            .pNext { nullptr },
            .flags {},
            .queueFamilyIndex { queueFamily },
            .queueCount { 1 },
            .pQueuePriorities { &queuePriority }
        };

        queueCreateInfos.push_back(deviceQueueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures {
        .samplerAnisotropy { VK_TRUE }
    };

    VkDeviceCreateInfo deviceCreateInfo {
        .sType { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .queueCreateInfoCount { static_cast<uint32_t>(queueCreateInfos.size()) },
        .pQueueCreateInfos { queueCreateInfos.data() },
        .enabledLayerCount { /* deprecated and ignored */ },
        .ppEnabledLayerNames { /* deprecated and ignored */ },
        .enabledExtensionCount { static_cast<uint32_t>(requiredExtensions.size()) },
        .ppEnabledExtensionNames { requiredExtensions.data() },
        .pEnabledFeatures { &deviceFeatures }
    };

    CHECK_VK(vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device), "Failed to create logical device.");

    vkGetDeviceQueue(m_device, m_graphicsQueueFamilyIndex, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, m_presentQueueFamilyIndex, 0, &m_presentQueue);
}

void VK::Device::CreateMemoryAllocator()
{
    VmaAllocatorCreateInfo allocatorCreateInfo {
        .physicalDevice { m_physicalDevice },
        .device { m_device },
        .instance { m_instance }
    };

    CHECK_VK(vmaCreateAllocator(&allocatorCreateInfo, &m_allocator), "Failed to create memory allocator");
}

inline void VK::Device::CreateCommandPool(void)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO },
        .pNext { nullptr },
        .flags { VK_COMMAND_POOL_CREATE_TRANSIENT_BIT },
        .queueFamilyIndex { m_graphicsQueueFamilyIndex }
    };

    CHECK_VK(vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_commandPool), "Failed to create command pool");
}
