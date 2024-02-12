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
    void Initialize(const VkDevice& device, VkDescriptorPoolCreateFlags createFlags, uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes);
    void Destroy(void);

public: // method
    DescriptorSet AllocateDescriptorSet(const VkDescriptorSetLayout* pSetLayouts) const;

public: // getter
    VkDescriptorPool GetHandle(void) const { return m_handle; }

private:
    VkDevice m_device { VK_NULL_HANDLE };

private:
    VkDescriptorPool m_handle { VK_NULL_HANDLE };
};
}
