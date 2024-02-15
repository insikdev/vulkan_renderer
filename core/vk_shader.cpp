#include "vk_shader.h"

VkResult VK::Shader::Init(VkDevice device, const std::vector<char>& code)
{
    assert(m_handle == VK_NULL_HANDLE);

    {
        m_device = device;
    }

    VkShaderModuleCreateInfo createInfo {
        .sType { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO },
        .pNext {},
        .flags {},
        .codeSize { code.size() },
        .pCode { reinterpret_cast<const uint32_t*>(code.data()) }
    };

    return vkCreateShaderModule(m_device, &createInfo, nullptr, &m_handle);
}

void VK::Shader::Destroy()
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyShaderModule(m_device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}