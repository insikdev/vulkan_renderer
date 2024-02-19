#include "vk_descriptor_set.h"

VK::DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept
    : m_handle { other.m_handle }
    , m_device { other.m_device }
    , m_descriptorPool { other.m_descriptorPool }
{
    other.m_handle = VK_NULL_HANDLE;
}

VK::DescriptorSet& VK::DescriptorSet::operator=(DescriptorSet&& other) noexcept
{
    if (this != &other) {
        m_handle = other.m_handle;
        m_device = other.m_device;
        m_descriptorPool = other.m_descriptorPool;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

VkResult VK::DescriptorSet::Init(VkDevice device, VkDescriptorPool descriptorPool, const VkDescriptorSetLayout* pSetLayouts)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
        m_descriptorPool = descriptorPool;
    }

    VkDescriptorSetAllocateInfo allocateInfo {
        .sType { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO },
        .pNext {},
        .descriptorPool { descriptorPool },
        .descriptorSetCount { 1 },
        .pSetLayouts { pSetLayouts }
    };

    return vkAllocateDescriptorSets(m_device, &allocateInfo, &m_handle);
}

void VK::DescriptorSet::Free(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkFreeDescriptorSets(m_device, m_descriptorPool, 1, &m_handle);
        m_handle = VK_NULL_HANDLE;
    }
}

void VK::DescriptorSet::WriteBuffer(uint32_t dstBinding, const VkDescriptorBufferInfo& bufferInfo)
{
    VkWriteDescriptorSet descriptorWrite {
        .sType { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET },
        .pNext { nullptr },
        .dstSet { m_handle },
        .dstBinding { dstBinding },
        .dstArrayElement {},
        .descriptorCount { 1 },
        .descriptorType { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
        .pImageInfo { /* ignored */ },
        .pBufferInfo { &bufferInfo },
        .pTexelBufferView { /* ignored */ }
    };

    vkUpdateDescriptorSets(m_device, 1, &descriptorWrite, 0, nullptr);
}

void VK::DescriptorSet::WriteImage(uint32_t dstBinding, const VkDescriptorImageInfo& imageInfo)
{
    VkWriteDescriptorSet write {
        .sType { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET },
        .pNext { nullptr },
        .dstSet { m_handle },
        .dstBinding { dstBinding },
        .dstArrayElement {},
        .descriptorCount { 1 },
        .descriptorType { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER },
        .pImageInfo { &imageInfo },
        .pBufferInfo { /* ignored */ },
        .pTexelBufferView { /* ignored */ }
    };

    vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
}
