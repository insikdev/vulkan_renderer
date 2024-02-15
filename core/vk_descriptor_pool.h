#pragma once

#include "common.h"

namespace VK {
class DescriptorSet;

class DescriptorPool {
public:
    DescriptorPool() = default;
    ~DescriptorPool() { Destroy(); }
    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool(DescriptorPool&&) noexcept;
    DescriptorPool& operator=(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&&) noexcept;

public:
    VkResult Init(VkDevice device, VkDescriptorPoolCreateFlags createFlags, uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes);
    void Destroy(void);

public:
    VkDescriptorPool GetHandle(void) const { return m_handle; }
    DescriptorSet AllocateDescriptorSet(const VkDescriptorSetLayout* pSetLayouts) const;

private:
    VkDevice m_device { VK_NULL_HANDLE };
    VkDescriptorPool m_handle { VK_NULL_HANDLE };
};
}
