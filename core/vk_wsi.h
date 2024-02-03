#pragma once

namespace VK {
class Instance;
class Surface;
class Device;
class Swapchain;

class WSI {
#ifdef _WIN32
public:
    WSI(const std::vector<const char*>& instanceLayers, const std::vector<const char*>& instanceExtensions, const std::vector<const char*>& deviceExtensions, HINSTANCE hinstance, HWND hwnd);
#endif

public:
    ~WSI();

public: // getter
    Device* GetDevice(void) const { return p_device; }
    Swapchain* GetSwapchain(void) const { return p_swapchain; }

private:
    Instance* p_instance;
    Surface* p_surface;
    Device* p_device;
    Swapchain* p_swapchain;
};
}
