#pragma once

#include "common.h"

namespace VK {
class Shader {
public:
    Shader() = default;
    ~Shader() { Destroy(); }
    Shader(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&) = delete;

public:
    VkResult Init(VkDevice device, const std::vector<char>& code);
    void Destroy(void);

public:
    VkShaderModule GetHandle(void) const { return m_handle; }

private:
    VkDevice m_device { VK_NULL_HANDLE };
    VkShaderModule m_handle { VK_NULL_HANDLE };
};
}
