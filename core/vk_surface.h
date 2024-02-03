#pragma once

namespace VK {
class Surface {
public:
#ifdef _WIN32
    Surface(VkInstance instance, HINSTANCE hinstance, HWND hwnd);
#endif
    ~Surface();

public: // getter
    VkSurfaceKHR GetHandle(void) const { return m_surface; }

private:
    VkInstance m_instance { VK_NULL_HANDLE };

private:
    VkSurfaceKHR m_surface { VK_NULL_HANDLE };
};
}
