#pragma once

namespace VK {
class Shader {
public:
    Shader(VkDevice device, const std::vector<char>& code);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

public:
    VkShaderModule GetHandle(void) const { return m_module; }

private:
    VkDevice m_device;
    VkShaderModule m_module;
};
}
