#include "vk_command_buffer.h"

VK::CommandBuffer::~CommandBuffer()
{
    Destroy();
}

VK::CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
    : m_handle { other.m_handle }
    , m_device { other.m_device }
    , m_commandPool { other.m_commandPool }
    , m_queueToSubmit { other.m_queueToSubmit }
{
    other.m_handle = VK_NULL_HANDLE;
}

VK::CommandBuffer& VK::CommandBuffer::operator=(CommandBuffer&& other) noexcept
{
    if (this != &other) {
        m_handle = other.m_handle;
        m_device = other.m_device;
        m_commandPool = other.m_commandPool;
        m_queueToSubmit = other.m_queueToSubmit;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

void VK::CommandBuffer::Initialize(const VkDevice& device, const VkCommandPool& commandPool, const VkQueue& queueToSubmit)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
        m_commandPool = commandPool;
        m_queueToSubmit = queueToSubmit;
    }

    VkCommandBufferAllocateInfo allocInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },
        .pNext { nullptr },
        .commandPool { m_commandPool },
        .level { VK_COMMAND_BUFFER_LEVEL_PRIMARY },
        .commandBufferCount { 1 }
    };

    CHECK_VK(vkAllocateCommandBuffers(m_device, &allocInfo, &m_handle), "Failed to allocate command buffer.");
}

void VK::CommandBuffer::Destroy(void)
{
    if (m_handle != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_handle);
        m_handle = VK_NULL_HANDLE;
    }
}

void VK::CommandBuffer::BeginRecording(VkCommandBufferUsageFlags usageFlags) const
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

void VK::CommandBuffer::EndRecording(void) const
{
    assert(m_handle != VK_NULL_HANDLE);

    CHECK_VK(vkEndCommandBuffer(m_handle), "");
}

void VK::CommandBuffer::Reset(void) const
{
    assert(m_handle != VK_NULL_HANDLE);

    CHECK_VK(vkResetCommandBuffer(m_handle, 0), "");
    // CommandBuffer must have been allocated from a pool that was created with the VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
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

    CHECK_VK(vkQueueSubmit(m_queueToSubmit, 1, &submitInfo, fence), "Failed to submit command buffer.");
    CHECK_VK(vkQueueWaitIdle(m_queueToSubmit), "");
}
