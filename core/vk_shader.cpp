#include "vk_shader.h"

VK::Shader::~Shader()
{
    Destroy();
}

void VK::Shader::Initialize(const VkDevice& device, const std::vector<char>& code)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
    }

    VkShaderModuleCreateInfo shaderModuleCreateInfo {
        .sType { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .codeSize { code.size() },
        .pCode { reinterpret_cast<const uint32_t*>(code.data()) }
    };

    CHECK_VK(vkCreateShaderModule(m_device, &shaderModuleCreateInfo, nullptr, &m_handle), "Failed to create shader module.");
}

void VK::Shader::Destroy()
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyShaderModule(m_device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}