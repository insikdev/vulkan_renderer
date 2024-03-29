#include "vk_descriptor_pool.h"
#include "vk_descriptor_set.h"

VK::DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
    : m_handle { other.m_handle }
    , m_device { other.m_device }
{
    other.m_handle = VK_NULL_HANDLE;
}

VK::DescriptorPool& VK::DescriptorPool::operator=(DescriptorPool&& other) noexcept
{
    if (this != &other) {
        m_handle = other.m_handle;
        m_device = other.m_device;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

VkResult VK::DescriptorPool::Init(VkDevice device, VkDescriptorPoolCreateFlags createFlags, uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
    }

    VkDescriptorPoolCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO },
        .pNext {},
        .flags { createFlags },
        .maxSets { maxSets },
        .poolSizeCount { static_cast<uint32_t>(poolSizes.size()) },
        .pPoolSizes { poolSizes.data() }
    };

    return vkCreateDescriptorPool(m_device, &createInfo, nullptr, &m_handle);
}

void VK::DescriptorPool::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VK::DescriptorSet VK::DescriptorPool::AllocateDescriptorSet(const VkDescriptorSetLayout* pSetLayouts) const
{
    VK::DescriptorSet descriptorSet;
    descriptorSet.Init(m_device, m_handle, pSetLayouts);

    return descriptorSet;
}
