#pragma once

class GUI {
public:
    GUI(GLFWwindow* pWindow, VkRenderPass& renderPass);
    ~GUI();

public:
    void Render(const VkCommandBuffer& commandBuffer);

private:
    VK::DescriptorPool m_descriptorPool;
};
