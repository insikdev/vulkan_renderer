#pragma once
#include <vector>
namespace VK {
class Instance {
public:
    Instance() = default;
    ~Instance();
    Instance(const Instance&) = delete;
    Instance(Instance&&) = delete;
    Instance& operator=(const Instance&) = delete;
    Instance& operator=(Instance&&) = delete;

public:
    void Initialize(const std::vector<const char*>& requiredLayers, const std::vector<const char*>& requiredExtensions);
    void Destroy(void);

public: // getter
    VkInstance GetHandle(void) const { return m_handle; }

private:
    VkInstance m_handle { VK_NULL_HANDLE };
};
}
