#include "pch.h"
#include "vk_device.h"
#include "vk_instance.h"
#include "query.h"

VK::Device::Device(const Instance* pInstance)
    : p_instance { pInstance }
{
    m_physicalDevice = SelectPhysicalDevice();
    CreateLogicalDevice();
}

VK::Device::~Device()
{
    vkDestroyDevice(m_device, nullptr);
}

VkPhysicalDevice VK::Device::SelectPhysicalDevice()
{
    std::vector<VkPhysicalDevice> devices = Query::GetPhysicalDevices(p_instance->GetHandle());

    if (devices.empty()) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support.");
    }

    if (devices.size() == 1) {
        return devices[0];
    }

    for (const auto& device : devices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            return device;
        }
    }

    throw std::runtime_error("Failed to select physical device.");
}

void VK::Device::CreateLogicalDevice()
{
    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo deviceQueueCreateInfo {
        .sType { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .queueFamilyIndex { 0 },
        .queueCount { 1 },
        .pQueuePriorities { &queuePriority }
    };

    VkPhysicalDeviceFeatures deviceFeatures {};

    VkDeviceCreateInfo deviceCreateInfo {
        .sType { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .queueCreateInfoCount { 1 },
        .pQueueCreateInfos { &deviceQueueCreateInfo },
        .enabledLayerCount {},
        .ppEnabledLayerNames {},
        .enabledExtensionCount {},
        .ppEnabledExtensionNames {},
        .pEnabledFeatures { &deviceFeatures }
    };

    CHECK_VK(vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device), "Failed to create logical device.");

    vkGetDeviceQueue(m_device, 0, 0, &m_queue);
}
