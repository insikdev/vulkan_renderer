#pragma once

namespace VK {
class Instance {
public:
    Instance();
    ~Instance();

public:
    VkInstance GetHandle(void) const { return m_instance; }

private:
    void CreateInstance(void);

private:
    VkInstance m_instance { VK_NULL_HANDLE };
};
}
