#pragma once

namespace VK {
class Surface {
public:
    Surface() = default;
    ~Surface();
    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;
    Surface(Surface&&) = delete;
    Surface& operator=(Surface&&) = delete;

public:
#ifdef _WIN32
    void Initialize(VkInstance instance, HINSTANCE hinstance, HWND hwnd);
#endif
    void Destroy(void);

public: // getter
    VkSurfaceKHR GetHandle(void) const { return m_handle; }

private:
    VkInstance m_instance;
    VkSurfaceKHR m_handle { VK_NULL_HANDLE };
};
}
