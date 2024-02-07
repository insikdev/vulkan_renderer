#include "vk_shader.h"
#include "vk_device.h"

VK::Shader::~Shader()
{
    Destroy();
}

void VK::Shader::Initialize(const Device* pDevice, const std::vector<char>& code)
{
    assert(m_handle == VK_NULL_HANDLE && pDevice != nullptr);

    {
        p_device = pDevice;
    }

    VkShaderModuleCreateInfo shaderModuleCreateInfo {
        .sType { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .codeSize { code.size() },
        .pCode { reinterpret_cast<const uint32_t*>(code.data()) }
    };

    CHECK_VK(vkCreateShaderModule(p_device->GetHandle(), &shaderModuleCreateInfo, nullptr, &m_handle), "Failed to create shader module.");
}

void VK::Shader::Destroy()
{
    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyShaderModule(p_device->GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
        p_device = nullptr;
    }
}