#pragma once

#include "common.h"
#include "vk_instance.h"
#include "vk_surface.h"
#include "vk_device.h"
#include "vk_swapchain.h"
#include "vk_command_pool.h"
#include "vk_memory_allocator.h"

namespace VK {
class WSI {
public:
    WSI() = default;
    ~WSI() { Destroy(); }
    WSI(const WSI&) = delete;
    WSI(WSI&&) = delete;
    WSI& operator=(const WSI&) = delete;
    WSI& operator=(WSI&&) = delete;

public:
    void Initialize(
        const VkApplicationInfo* pApplicationInfo,
        const std::vector<const char*>& instanceLayers,
        const std::vector<const char*>& instanceExtensions,
        const std::vector<const char*>& deviceExtensions,
        const HINSTANCE& hinstance,
        const HWND& hwnd);
    void Destroy(void);

public: // getter
    const Device* GetDevice(void) const { return &m_device; }
    Swapchain* GetSwapchain(void) { return &m_swapchain; }
    const CommandPool* GetCommandPool(void) const { return &m_commandPool; }
    const MemoryAllocator* GetMemoryAllocator(void) const { return &m_memoryAllocator; }

private:
    bool m_initialized { false };
    Instance m_instance {};
    Surface m_surface {};
    Device m_device {};
    Swapchain m_swapchain {};
    CommandPool m_commandPool {};
    MemoryAllocator m_memoryAllocator {};
};
}
