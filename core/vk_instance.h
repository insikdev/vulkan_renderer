#pragma once

namespace VK {
class Instance {
public:
    Instance(const std::vector<const char*>& requiredLayers, const std::vector<const char*>& requiredExtensions);
    ~Instance();

public: // getter
    VkInstance GetHandle(void) const { return m_instance; }

private:
    VkInstance m_instance { VK_NULL_HANDLE };
};
}
