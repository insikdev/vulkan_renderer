#include "pch.h"
#include "vk_device.h"
#include "vk_instance.h"
#include "vk_surface.h"
#include "query.h"
#include "utils.h"
#include "vk_command_buffer.h"
#include "check_vk.h"

VK::Device::~Device()
{
    Destroy();
}

void VK::Device::Initialize(const Instance* pInstance, const Surface* pSurface, const std::vector<const char*>& requiredExtensions)
{
    assert(m_device == VK_NULL_HANDLE && pInstance != VK_NULL_HANDLE && pSurface != VK_NULL_HANDLE);

    {
        p_instance = pInstance;
        p_surface = pSurface;
    }

    m_physicalDevice = SelectPhysicalDevice();

    if (Utils::CheckExtensionSupport(requiredExtensions, Query::GetDeviceExtensions(m_physicalDevice)) == false) {
        throw std::runtime_error("Required extensions are not supported.");
    }

    SelectQueueIndex(p_surface->GetHandle());
    CreateLogicalDevice(requiredExtensions);

    m_allocator.Initialize(p_instance, this);
    m_commandPool.Initialize(this);
}

void VK::Device::Destroy(void)
{
    if (m_device != VK_NULL_HANDLE) {
        m_commandPool.Destroy();
        m_allocator.Destroy();
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
        p_instance = nullptr;
        p_surface = nullptr;
    }
}

VkPhysicalDevice VK::Device::SelectPhysicalDevice()
{
    const auto& devices = Query::GetPhysicalDevices(p_instance->GetHandle());

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
