#pragma once

namespace VK {
class Instance;

class WinSurface {
public:
    WinSurface(const Instance* pInstance, HINSTANCE hinstance, HWND hwnd);
    ~WinSurface();

public:
    VkSurfaceKHR GetHandle(void) const { return m_surface; }

private:
    const Instance* p_instance;
    VkSurfaceKHR m_surface;
};
}
