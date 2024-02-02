#pragma once

namespace VK {
class Shader {
public:
    Shader(VkDevice device, const std::vector<char>& code);
    ~Shader();

public:
    VkShaderModule GetHandle(void) const { return m_module; }

private:
    VkDevice m_device;
    VkShaderModule m_module;
};
}
