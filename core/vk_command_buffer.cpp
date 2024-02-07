#include "vk_command_buffer.h"
#include "vk_device.h"
#include "vk_command_pool.h"

VK::CommandBuffer::~CommandBuffer()
{
    Destroy();
}

VK::CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
    : m_handle { other.m_handle }
    , p_device { other.p_device }
    , p_commandPool { other.p_commandPool }
{
    other.m_handle = VK_NULL_HANDLE;
    other.p_device = nullptr;
    other.p_commandPool = nullptr;
}

VK::CommandBuffer& VK::CommandBuffer::operator=(CommandBuffer&& other) noexcept
{
    if (this != &other) {
        m_handle = other.m_handle;
        p_device = other.p_device;
        p_commandPool = other.p_commandPool;

        other.m_handle = VK_NULL_HANDLE;
        other.p_device = nullptr;
        other.p_commandPool = nullptr;
    }

    return *this;
}

void VK::CommandBuffer::Initialize(const Device* pDevice, const CommandPool* pCommandPool)
{
    assert(m_handle == VK_NULL_HANDLE && pDevice != nullptr && pCommandPool != nullptr);

    {
        p_device = pDevice;
        p_commandPool = pCommandPool;
    }

    VkCommandBufferAllocateInfo allocInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },
        .pNext { nullptr },
        .commandPool { p_commandPool->GetHandle() },
        .level { VK_COMMAND_BUFFER_LEVEL_PRIMARY },
        .commandBufferCount { 1 }
    };

    CHECK_VK(vkAllocateCommandBuffers(p_device->GetHandle(), &allocInfo, &m_handle), "Failed to allocate command buffer.");
}

void VK::CommandBuffer::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(p_device->GetHandle(), p_commandPool->GetHandle(), 1, &m_handle);
        m_handle = VK_NULL_HANDLE;
        p_device = nullptr;
        p_commandPool = nullptr;
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
