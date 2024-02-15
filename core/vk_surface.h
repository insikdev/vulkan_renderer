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
    VkResult Init(VkInstance instance, HINSTANCE hinstance, HWND hwnd);
    void Destroy(void);

public:
    VkSurfaceKHR GetHandle(void) const { return m_handle; }

private:
    VkInstance m_instance { VK_NULL_HANDLE };
    VkSurfaceKHR m_handle { VK_NULL_HANDLE };
};
}
