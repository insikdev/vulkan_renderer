#pragma once

namespace VK {
class Instance {
public:
    Instance();
    ~Instance();

private:
    VkInstance m_instance;
};
}
