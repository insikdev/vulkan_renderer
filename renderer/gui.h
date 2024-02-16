#pragma once

class GUI {
public:
    GUI() = default;
    ~GUI() { Destroy(); }
    GUI(const GUI&) = delete;
    GUI(GUI&&) = delete;
    GUI& operator=(const GUI&) = delete;
    GUI& operator=(GUI&&) = delete;

public:
    void Init(GLFWwindow* pWindow, ImGui_ImplVulkan_InitInfo* info, VkRenderPass& renderPass, GuiOptions* pOptions);
    void Destroy(void);
    void Render(VkCommandBuffer commandBuffer);

private:
    GuiOptions* p_options;
};
