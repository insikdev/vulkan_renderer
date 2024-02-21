#include "vk_queue.h"

void VK::Queue::Init(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_queueFamilyIndex = queueFamilyIndex;
        m_queueIndex = queueIndex;
    }

    vkGetDeviceQueue(device, m_queueFamilyIndex, m_queueIndex, &m_handle);
}

VkResult VK::Queue::WaitIdle(void) const
{
    assert(m_handle != VK_NULL_HANDLE);

    return vkQueueWaitIdle(m_handle);
}

VkResult VK::Queue::Submit(const std::vector<VkSubmitInfo>& submitInfos, VkFence fence) const
{
    assert(m_handle != VK_NULL_HANDLE);

    return vkQueueSubmit(m_handle, static_cast<uint32_t>(submitInfos.size()), submitInfos.data(), fence);
}

VkResult VK::Queue::Submit(const std::vector<VkCommandBuffer>& commandBuffers, VkFence fence) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VkSubmitInfo submitInfo {
        .sType { VK_STRUCTURE_TYPE_SUBMIT_INFO },
        .pNext {},
        .waitSemaphoreCount {},
        .pWaitSemaphores {},
        .pWaitDstStageMask {},
        .commandBufferCount { static_cast<uint32_t>(commandBuffers.size()) },
        .pCommandBuffers { commandBuffers.data() },
        .signalSemaphoreCount {},
        .pSignalSemaphores {}
    };

    return vkQueueSubmit(m_handle, 1, &submitInfo, fence);
}

VkResult VK::Queue::Present(const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkSwapchainKHR>& swapchains, const uint32_t* pImageIndices) const
{
    assert(m_handle != VK_NULL_HANDLE);

    VkPresentInfoKHR presentInfo {
        .sType { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR },
        .pNext { nullptr },
        .waitSemaphoreCount { static_cast<uint32_t>(waitSemaphores.size()) },
        .pWaitSemaphores { waitSemaphores.data() },
        .swapchainCount { static_cast<uint32_t>(swapchains.size()) },
        .pSwapchains { swapchains.data() },
        .pImageIndices { pImageIndices },
        .pResults { nullptr },
    };

    return vkQueuePresentKHR(m_handle, &presentInfo);
}
