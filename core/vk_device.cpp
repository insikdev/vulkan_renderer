#include "vk_device.h"
#include "query.h"
#include "vk_command_pool.h"
#include "vk_descriptor_pool.h"

void VK::Device::Initialize(const VkInstance& instance, const VkSurfaceKHR& surface, const std::vector<const char*>& requiredExtensions)
{
    assert(m_device == VK_NULL_HANDLE);

    m_physicalDevice = SelectPhysicalDevice(instance);
    SelectQueueIndex(surface);
    CreateLogicalDevice(requiredExtensions);
}

void VK::Device::Destroy(void)
{
    if (m_device != VK_NULL_HANDLE) {
        vkDestroyDevice(m_device, nullptr);
        m_physicalDevice = VK_NULL_HANDLE;
        m_device = VK_NULL_HANDLE;
        m_graphicsQueue = VK_NULL_HANDLE;
        m_presentQueue = VK_NULL_HANDLE;
    }
}

VK::CommandPool VK::Device::CreateCommandPool(VkCommandPoolCreateFlags createFlags) const
{
    VK::CommandPool commandPool;
    commandPool.Initialize(m_device, m_graphicsQueueFamilyIndex, createFlags);

    return commandPool;
}

VK::DescriptorPool VK::Device::CreateDescriptorPool(uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes, VkDescriptorPoolCreateFlags createFlags) const
{
    VK::DescriptorPool descriptorPool;
    descriptorPool.Initialize(m_device, createFlags, maxSets, poolSizes);

    return descriptorPool;
}

VkPhysicalDevice VK::Device::SelectPhysicalDevice(const VkInstance& instance)
{
    const auto& devices = Query::GetPhysicalDevices(instance);

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

void VK::Device::SelectQueueIndex(const VkSurfaceKHR& surface)
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
