#include "pch.h"
#include "vk_shader.h"

VK::Shader::Shader(VkDevice device, const std::vector<char>& code)
    : m_device { device }
{
    VkShaderModuleCreateInfo shaderModuleCreateInfo {
        .sType { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .codeSize { code.size() },
        .pCode { reinterpret_cast<const uint32_t*>(code.data()) }
    };

    CHECK_VK(vkCreateShaderModule(m_device, &shaderModuleCreateInfo, nullptr, &m_module), "Failed to create shader module.");
}

VK::Shader::~Shader()
{
    vkDestroyShaderModule(m_device, m_module, nullptr);
}
