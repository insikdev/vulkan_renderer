#pragma once

#include "vk_resource.h"

namespace VK {
class Buffer : public Resource {
    friend class Device;

public:
    Buffer(VmaAllocator allocator);
    ~Buffer() override;

public:
    void CopyDataToDevice(void* pSrc, VkDeviceSize size);

public: // getter
    VkBuffer GetHandle(void) const { return m_buffer; }

private:
    VkBuffer m_buffer { VK_NULL_HANDLE };
};
}
