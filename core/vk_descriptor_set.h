#pragma once

#include "common.h"

namespace VK {
class DescriptorSet {
public:
    DescriptorSet() = default;
    ~DescriptorSet() = default;
    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet(DescriptorSet&&) noexcept;
    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&&) noexcept;

public:
    VkResult Init(VkDevice device, VkDescriptorPool descriptorPool, const VkDescriptorSetLayout* pSetLayouts);
    void Free(void);

public:
    VkDescriptorSet GetHandle(void) const { return m_handle; }
    void WriteBuffer(uint32_t dstBinding, const VkDescriptorBufferInfo& bufferInfo);
    void WriteImage(uint32_t dstBinding, const VkDescriptorImageInfo& imageInfo);

private:
    VkDevice m_device { VK_NULL_HANDLE };
    VkDescriptorPool m_descriptorPool { VK_NULL_HANDLE };
    VkDescriptorSet m_handle { VK_NULL_HANDLE };
};
}
