#pragma once

#include "common.h"

namespace VK {
class Surface {
public:
    Surface() = default;
    ~Surface() { Destroy(); }
    Surface(const Surface&) = delete;
    Surface(Surface&&) = delete;
    Surface& operator=(const Surface&) = delete;
    Surface& operator=(Surface&&) = delete;

public:
    void Initialize(const VkInstance& instance, const HINSTANCE& hinstance, const HWND& hwnd);
    void Destroy(void);

public: // getter
    VkSurfaceKHR GetHandle(void) const { return m_handle; }

private:
    VkInstance m_instance { VK_NULL_HANDLE };

private:
    VkSurfaceKHR m_handle { VK_NULL_HANDLE };
};
}
