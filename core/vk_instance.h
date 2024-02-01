#pragma once

namespace VK {
class Instance {
public:
    Instance(const std::vector<const char*>& requiredLayers, const std::vector<const char*>& requiredExtensions);
    ~Instance();

public:
    VkInstance GetHandle(void) const { return m_instance; }

private:
    void CreateInstance(const std::vector<const char*>& requiredLayers, const std::vector<const char*>& requiredExtensions);

private:
    VkInstance m_instance { VK_NULL_HANDLE };
};
}
