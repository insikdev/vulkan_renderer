#include "pch.h"
#include "vk_device.h"
#include "query.h"
#include "utils.h"
#include "vk_command_pool.h"
#include "vk_command_buffer.h"
#include "check_vk.h"

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

    m_commandPool.Initialize(m_device, m_graphicsQueueFamilyIndex);
}

VK::Device::~Device()
{
    m_commandPool.Destroy();
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
    VK::CommandBuffer commandBuffer = m_commandPool.AllocateCommandBuffer();
    commandBuffer.BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferCopy copyRegion {
        .srcOffset {},
        .dstOffset {},
        .size { size }
    };

    vkCmdCopyBuffer(commandBuffer.GetHandle(), src, dst, 1, &copyRegion);

    commandBuffer.EndRecording();
    commandBuffer.Submit(m_graphicsQueue);
}

void VK::Device::CopyDataToDevice(VmaAllocation allocation, void* pSrc, VkDeviceSize size)
{
    void* mappedData;
    vmaMapMemory(m_allocator, allocation, &mappedData);
    memcpy(mappedData, pSrc, size);
    vmaUnmapMemory(m_allocator, allocation);
}

VK::Image VK::Device::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage)
{
    VkImageCreateInfo imageInfo {
        .sType { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .imageType { VK_IMAGE_TYPE_2D },
        .format { format },
        .extent { width, height, 1 },
        .mipLevels { 1 },
        .arrayLayers { 1 },
        .samples { VK_SAMPLE_COUNT_1_BIT },
        .tiling { tiling },
        .usage { usage },
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
    VK::CommandBuffer commandBuffer = m_commandPool.AllocateCommandBuffer();
    commandBuffer.BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

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

    vkCmdCopyBufferToImage(commandBuffer.GetHandle(), buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    commandBuffer.EndRecording();
    commandBuffer.Submit(m_graphicsQueue);
}

VkImageView VK::Device::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkComponentMapping components {
        .r { VK_COMPONENT_SWIZZLE_IDENTITY },
        .g { VK_COMPONENT_SWIZZLE_IDENTITY },
        .b { VK_COMPONENT_SWIZZLE_IDENTITY },
        .a { VK_COMPONENT_SWIZZLE_IDENTITY }
    };

    VkImageSubresourceRange subresourceRange {
        .aspectMask { aspectFlags },
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

    CHECK_VK(vmaCreateAllocator(&allocatorCreateInfo, &m_allocator), "Failed to create memory allocator.");
}
