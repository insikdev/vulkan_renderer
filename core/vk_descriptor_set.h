#pragma once

#include "common.h"

namespace VK {
class DescriptorSet {
public:
    DescriptorSet() = default;
    ~DescriptorSet() { Destroy(); }
    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet(DescriptorSet&&) noexcept;
    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&&) noexcept;

public:
    void Initialize(const VkDevice& device, const VkDescriptorPool& descriptorPool, const VkDescriptorSetLayout* pSetLayouts);
    void Destroy(void);

public: // method
    void WriteBuffer(uint32_t dstBinding, const VkDescriptorBufferInfo& bufferInfo);
    void WriteImage(uint32_t dstBinding, const VkDescriptorImageInfo& imageInfo);

public: // getter
    VkDescriptorSet GetHandle(void) const { return m_handle; }

private:
    VkDevice m_device { VK_NULL_HANDLE };
    VkDescriptorPool m_descriptorPool { VK_NULL_HANDLE };

private:
    VkDescriptorSet m_handle { VK_NULL_HANDLE };
};
}
