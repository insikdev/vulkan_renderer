#include "pch.h"
#include "vk_buffer.h"
#include "vk_device.h"
#include "vk_command_pool.h"
#include "vk_command_buffer.h"

VK::Buffer::~Buffer()
{
    Destroy();
}

void VK::Buffer::Initialize(const MemoryAllocator* pAllocator, VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags)
{
    assert(m_handle == VK_NULL_HANDLE && pAllocator != nullptr);

    {
        p_allocator = pAllocator;
    }

    p_allocator->CreateBuffer(this, size, usage, allocationFlags);
}

void VK::Buffer::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        p_allocator->DestroyBuffer(this);
        m_handle = VK_NULL_HANDLE;
        p_allocator = nullptr;
    }
}

void VK::Buffer::CopyData(void* pSrc, VkDeviceSize size)
{
    assert(m_allocation != VK_NULL_HANDLE);

    memcpy(p_allocator->Map(this), pSrc, size);
    p_allocator->Unmap(this);
}

void VK::Buffer::CopyBufferToBuffer(const Device* pDevice, const Buffer* src, const Buffer* dst, VkDeviceSize size)
{
    VK::CommandBuffer commandBuffer = pDevice->GetCommandPool()->AllocateCommandBuffer();

    commandBuffer.BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferCopy copyRegion {
        .srcOffset {},
        .dstOffset {},
        .size { size }
    };

    vkCmdCopyBuffer(commandBuffer.GetHandle(), src->m_handle, dst->m_handle, 1, &copyRegion);

    commandBuffer.EndRecording();
    commandBuffer.Submit(pDevice->GetGrahpicsQueue());
}
