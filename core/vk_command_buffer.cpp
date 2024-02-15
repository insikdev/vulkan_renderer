#include "vk_command_buffer.h"

VK::CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
    : m_handle { other.m_handle }
    , m_device { other.m_device }
    , m_commandPool { other.m_commandPool }
    , m_queue { other.m_queue }
{
    other.m_handle = VK_NULL_HANDLE;
}

VK::CommandBuffer& VK::CommandBuffer::operator=(CommandBuffer&& other) noexcept
{
    if (this != &other) {
        m_handle = other.m_handle;
        m_device = other.m_device;
        m_commandPool = other.m_commandPool;
        m_queue = other.m_queue;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

VkResult VK::CommandBuffer::Init(VkDevice device, VkCommandPool commandPool, VkQueue queue)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
        m_commandPool = commandPool;
        m_queue = queue;
    }

    VkCommandBufferAllocateInfo allocateInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },
        .pNext { nullptr },
        .commandPool { m_commandPool },
        .level { VK_COMMAND_BUFFER_LEVEL_PRIMARY },
        .commandBufferCount { 1 }
    };

    return vkAllocateCommandBuffers(m_device, &allocateInfo, &m_handle);
}

void VK::CommandBuffer::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_handle);
        m_handle = VK_NULL_HANDLE;
    }
}

VkResult VK::CommandBuffer::BeginRecording(VkCommandBufferUsageFlags usageFlags) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VkCommandBufferBeginInfo beginInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
        .pNext { nullptr },
        .flags { usageFlags },
        .pInheritanceInfo {}
    };

    return vkBeginCommandBuffer(m_handle, &beginInfo);
}

VkResult VK::CommandBuffer::EndRecording(void) const
{
    assert(m_handle != VK_NULL_HANDLE);

    return vkEndCommandBuffer(m_handle);
}

// CommandBuffer must have been allocated from a pool that was created with the VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
VkResult VK::CommandBuffer::Reset(void) const
{
    assert(m_handle != VK_NULL_HANDLE);

    return vkResetCommandBuffer(m_handle, 0);
}

void VK::CommandBuffer::Submit() const
{
    Submit({}, {}, VK_NULL_HANDLE);
}

void VK::CommandBuffer::Submit(const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkSemaphore>& signalSemaphores, VkFence fence) const
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

    CHECK_VK(vkQueueSubmit(m_queue, 1, &submitInfo, fence), "Failed to submit command buffer.");
    CHECK_VK(vkQueueWaitIdle(m_queue), "");
}
