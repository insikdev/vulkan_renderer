#pragma once

namespace VK {
class Instance;

class Surface {
public:
    Surface() = default;
    ~Surface();
    Surface(const Surface&) = delete;
    Surface(Surface&&) = delete;
    Surface& operator=(const Surface&) = delete;
    Surface& operator=(Surface&&) = delete;

public:
#ifdef _WIN32
    void Initialize(const Instance* pInstance, HINSTANCE hinstance, HWND hwnd);
#endif
    void Destroy(void);

public: // getter
    VkSurfaceKHR GetHandle(void) const { return m_handle; }

private:
    const Instance* p_instance;
    VkSurfaceKHR m_handle { VK_NULL_HANDLE };
};
}
