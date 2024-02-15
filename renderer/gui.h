#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

class GUI {
public:
    GUI(GLFWwindow* pWindow, VK::WSI* pWSI, VkRenderPass& renderPass);
    ~GUI();

public:
    void Render(const VkCommandBuffer& commandBuffer);

private:
    VK::WSI* p_wsi;
    VK::DescriptorPool m_descriptorPool;
    VK::CommandPool m_commandPool;
};
