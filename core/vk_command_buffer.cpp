#include "pch.h"
#include "vk_command_buffer.h"
#include "check_vk.h"

VK::CommandBuffer::~CommandBuffer()
{
    Free();
}

void VK::CommandBuffer::Initialize(VkDevice device, VkCommandPool commandPool)
{
    assert(m_handle == VK_NULL_HANDLE && device != VK_NULL_HANDLE && commandPool != VK_NULL_HANDLE);

    m_device = device;
    m_commandPool = commandPool;

    VkCommandBufferAllocateInfo allocInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },
        .pNext { nullptr },
        .commandPool { m_commandPool },
        .level { VK_COMMAND_BUFFER_LEVEL_PRIMARY },
        .commandBufferCount { 1 }
    };

    CHECK_VK(vkAllocateCommandBuffers(device, &allocInfo, &m_handle), "");
}

void VK::CommandBuffer::Free(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_handle);
        m_handle = VK_NULL_HANDLE;
    }
}

void VK::CommandBuffer::BeginRecording(VkCommandBufferUsageFlags usageFlags)
{
    assert(m_handle != VK_NULL_HANDLE);

    VkCommandBufferBeginInfo beginInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
        .pNext { nullptr },
        .flags { usageFlags },
        .pInheritanceInfo {}
    };

    CHECK_VK(vkBeginCommandBuffer(m_handle, &beginInfo), "");
}

void VK::CommandBuffer::EndRecording(void)
{
    assert(m_handle != VK_NULL_HANDLE);

    CHECK_VK(vkEndCommandBuffer(m_handle), "");
}

void VK::CommandBuffer::Reset(void)
{
    assert(m_handle != VK_NULL_HANDLE);

    CHECK_VK(vkResetCommandBuffer(m_handle, 0), "");
    // CommandBuffer must have been allocated from a pool that was created with the VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
}

void VK::CommandBuffer::Submit(VkQueue queue)
{
    Submit(queue, {}, {}, VK_NULL_HANDLE);
}

void VK::CommandBuffer::Submit(VkQueue queue, const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkSemaphore>& signalSemaphores, VkFence fence)
{
    assert(m_handle != VK_NULL_HANDLE);

    VkPipelineStageFlags waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo {
        .sType { VK_STRUCTURE_TYPE_SUBMIT_INFO },
        .pNext { nullptr },
        .waitSemaphoreCount { static_cast<uint32_t>(waitSemaphores.size()) },
        .pWaitSemaphores { waitSemaphores.data() },
        .pWaitDstStageMask { &waitStages },
        .commandBufferCount { 1 },
        .pCommandBuffers { &m_handle },
        .signalSemaphoreCount { static_cast<uint32_t>(signalSemaphores.size()) },
        .pSignalSemaphores { signalSemaphores.data() }
    };

    CHECK_VK(vkQueueSubmit(queue, 1, &submitInfo, fence), "");
    CHECK_VK(vkQueueWaitIdle(queue), "");
}
