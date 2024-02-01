#pragma once

namespace VK {
class Instance {
public:
    Instance(const std::vector<const char*>& layers, const std::vector<const char*>& extensions);
    ~Instance();

public:
    VkInstance GetHandle(void) const { return m_instance; }

private:
    void CreateInstance(const std::vector<const char*>& layers, const std::vector<const char*>& extensions);

private:
    VkInstance m_instance { VK_NULL_HANDLE };
};
}
